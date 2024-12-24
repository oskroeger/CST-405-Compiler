#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h" // for TAC definition

// --------------------------------------------------------------------------
// 1) Data structures for register/spill management
// --------------------------------------------------------------------------

// We'll track each distinct TAC temp in a table up to some limit.
// If we run out of "real" registers, we spill to a label in .data
// For real compilers, you'd do liveness analysis + stack-based spills, etc.

#define MAX_REGS 10  // number of MIPS registers we'll use for temps ($t0..$t9)
#define MAX_TEMPS 200

typedef struct {
    char* tempName;        // e.g. "t6"
    const char* assignedReg;   // e.g. "$t3" or NULL if spilled
    int spilled;           // 0 = in register, 1 = spilled to memory
} TempBinding;

static TempBinding g_tempBindings[MAX_TEMPS];
static int g_numTempBindings = 0;

// Next free register index in [0..MAX_REGS-1]
static int g_nextRegIndex = 0;

// We'll store the names of user-defined variables and spilled temps
// so we can declare them in .data later.
static char g_globalNames[500][64];
static int g_numGlobals = 0;

// Helper to see if a name is already in g_globalNames
static int isGlobalKnown(const char* name) {
    for (int i = 0; i < g_numGlobals; i++) {
        if (!strcmp(g_globalNames[i], name)) {
            return 1; // found
        }
    }
    return 0;
}

// Add a name to the global or spilled set
static void addGlobalName(const char* name) {
    if (!name) return;
    if (isGlobalKnown(name)) return;
    strcpy(g_globalNames[g_numGlobals++], name);
}

// Return 1 if `str` is "t<number>", else 0
static int isTempName(const char* str) {
    if (!str) return 0;
    if (str[0] != 't') return 0;
    // check digits
    for (int i = 1; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// Return the binding for a tempName, or NULL if not found
static TempBinding* lookupBinding(const char* tempName) {
    for (int i = 0; i < g_numTempBindings; i++) {
        if (!strcmp(g_tempBindings[i].tempName, tempName)) {
            return &g_tempBindings[i];
        }
    }
    return NULL;
}

// Assign a fresh register or spill
// - If we still have free registers, assign one
// - Otherwise mark spilled=1
static void allocateBinding(const char* tempName) {
    if (g_numTempBindings >= MAX_TEMPS) {
        fprintf(stderr, "[ERROR] Too many distinct temps\n");
        exit(1);
    }

    g_tempBindings[g_numTempBindings].tempName = strdup(tempName);
    g_tempBindings[g_numTempBindings].assignedReg = NULL;
    g_tempBindings[g_numTempBindings].spilled = 0;

    // If we still have free regs, assign one
    if (g_nextRegIndex < MAX_REGS) {
        // assign $t<g_nextRegIndex>
        static char regNameBuf[8];
        sprintf(regNameBuf, "$t%d", g_nextRegIndex);
        g_tempBindings[g_numTempBindings].assignedReg = strdup(regNameBuf);
        g_tempBindings[g_numTempBindings].spilled = 0;
        g_nextRegIndex++;
    } else {
        // Spill
        g_tempBindings[g_numTempBindings].assignedReg = NULL;
        g_tempBindings[g_numTempBindings].spilled = 1;

        // Mark that we need "var_tXX" in .data
        char labelBuf[64];
        sprintf(labelBuf, "var_%s", tempName);
        addGlobalName(labelBuf);
    }

    g_numTempBindings++;
}

// Return the register if the temp is in a register, or NULL if spilled
static const char* getRegIfAny(const char* tempName) {
    TempBinding* b = lookupBinding(tempName);
    if (!b) return NULL;
    if (b->spilled) return NULL;
    return b->assignedReg;  // e.g. "$t3"
}

// Return 1 if the given temp is spilled, else 0
static int isSpilled(const char* tempName) {
    TempBinding* b = lookupBinding(tempName);
    if (!b) return 0;
    return b->spilled;
}

// Return label name for a spilled temp, e.g. "var_t6"
static char* getSpillLabel(const char* tempName) {
    char* buf = (char*)malloc(64);
    sprintf(buf, "var_%s", tempName);
    return buf;
}

// For user-defined variables like "x", we store them as "var_x" in .data
// But we only do that if it's not recognized as a temp
static char* makeGlobalName(const char* var) {
    if (!var) return NULL;
    if (isTempName(var)) {
        // The "spilled" mechanism handles those
        if (isSpilled(var)) {
            return getSpillLabel(var);
        } else {
            // It's a temp in a register => no global name needed
            return NULL;
        }
    }
    // Non-temp => user variable => "var_x"
    char* buf = (char*)malloc(strlen(var) + 5);
    sprintf(buf, "var_%s", var);
    return buf;
}

// --------------------------------------------------------------------------
// 2) The "public" function to retrieve a register or spill a temp
// --------------------------------------------------------------------------

static void ensureTempBinding(const char* name) {
    if (!name) return;
    if (!isTempName(name)) {
        // user variable or immediate => no binding
        return;
    }
    TempBinding* b = lookupBinding(name);
    if (!b) {
        allocateBinding(name);
    }
}

// --------------------------------------------------------------------------
// 3) collectGlobalVars
// --------------------------------------------------------------------------

static void collectGlobalVars(TAC* tac, FILE* out) {
    // 1) Ensure we allocate bindings for all temps
    for (TAC* c = tac; c; c = c->next) {
        if (c->arg1) ensureTempBinding(c->arg1);
        if (c->arg2) ensureTempBinding(c->arg2);
        if (c->result) ensureTempBinding(c->result);
    }

    // 2) For each instruction, if there's a user-defined var, add to .data
    for (TAC* c = tac; c; c = c->next) {
        const char* op = c->operator ? c->operator : "";
        
        // For assignment or write/load, etc.
        if (!strcmp(op, "=") && c->result) {
            if (!isTempName(c->result)) {
                char* g = makeGlobalName(c->result);
                if (g) { addGlobalName(g); free(g); }
            }
        }
        if (!strcmp(op, "write") && c->arg1) {
            if (!isTempName(c->arg1)) {
                char* g = makeGlobalName(c->arg1);
                if (g) { addGlobalName(g); free(g); }
            }
        }
        if (!strcmp(op, "load") && c->arg1) {
            if (!isTempName(c->arg1)) {
                char* g = makeGlobalName(c->arg1);
                if (g) { addGlobalName(g); free(g); }
            }
        }
        // For +, -, *, / => check if arg1/arg2/result are user variables
        if (!strcmp(op, "+") || !strcmp(op, "-") ||
            !strcmp(op, "*") || !strcmp(op, "/")) {
            if (c->arg1 && !isTempName(c->arg1)) {
                char* g = makeGlobalName(c->arg1);
                if (g) { addGlobalName(g); free(g); }
            }
            if (c->arg2 && !isTempName(c->arg2)) {
                char* g = makeGlobalName(c->arg2);
                if (g) { addGlobalName(g); free(g); }
            }
            if (c->result && !isTempName(c->result)) {
                char* g = makeGlobalName(c->result);
                if (g) { addGlobalName(g); free(g); }
            }
        }
    }

    // 3) Emit .data
    fprintf(out, ".data\n");
    for (int i = 0; i < g_numGlobals; i++) {
        fprintf(out, "%s: .word 0\n", g_globalNames[i]);
    }
    fprintf(out, "\n.text\n");
}

// --------------------------------------------------------------------------
// 4) Load/store helpers
// --------------------------------------------------------------------------

static const char* loadArg(const char* arg, const char* scratch, FILE* out) {
    if (!arg) return scratch;
    // 1) integer literal?
    char* endp;
    long val = strtol(arg, &endp, 10);
    if (*endp == '\0') {
        // immediate
        fprintf(out, "li %s, %ld\n", scratch, val);
        return scratch;
    }

    // 2) temp?
    if (isTempName(arg)) {
        const char* r = getRegIfAny(arg);
        if (r) {
            // in register => move scratch, r
            fprintf(out, "move %s, %s\n", scratch, r);
            return scratch;
        } else if (isSpilled(arg)) {
            // spilled => la $s7, var_tXX / lw scratch, 0($s7)
            char* spillLabel = getSpillLabel(arg);
            fprintf(out, "la $s7, %s\n", spillLabel);
            fprintf(out, "lw %s, 0($s7)\n", scratch);
            free(spillLabel);
            return scratch;
        }
    }

    // 3) user variable => var_x
    char* g = makeGlobalName(arg);
    if (g) {
        fprintf(out, "la $s7, %s\n", g);
        fprintf(out, "lw %s, 0($s7)\n", scratch);
        free(g);
    }
    return scratch;
}

static void storeResult(const char* dest, const char* srcReg, FILE* out) {
    if (!dest) return;

    if (isTempName(dest)) {
        const char* r = getRegIfAny(dest);
        if (r) {
            // move r, srcReg
            fprintf(out, "move %s, %s\n", r, srcReg);
            return;
        } else if (isSpilled(dest)) {
            char* spillLabel = getSpillLabel(dest);
            fprintf(out, "la $s7, %s\n", spillLabel);
            fprintf(out, "sw %s, 0($s7)\n", srcReg);
            free(spillLabel);
            return;
        }
    }

    // else => user var "var_x"
    char* g = makeGlobalName(dest);
    if (g) {
        fprintf(out, "la $s7, %s\n", g);
        fprintf(out, "sw %s, 0($s7)\n", srcReg);
        free(g);
    }
}

// --------------------------------------------------------------------------
// 5) Main code generator
// --------------------------------------------------------------------------

void generateMIPSFromTAC(TAC* tacHead, const char* outputFilename) {
    fprintf(stderr, "DEBUG: Using the FIXED code generator!\n");
    FILE* out = fopen(outputFilename, "w");
    if (!out) {
        fprintf(stderr, "[ERROR] Could not open %s\n", outputFilename);
        return;
    }

    // Reset global state
    g_numTempBindings = 0;
    g_nextRegIndex = 0;
    g_numGlobals = 0;

    // 1) Collect global vars + do register assignment/spilling
    collectGlobalVars(tacHead, out);

    // 2) Start main
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");

    // 3) Emit code for each TAC
    for (TAC* c = tacHead; c; c = c->next) {
        const char* op = c->operator ? c->operator : "";

        if (!strcmp(op, "=")) {
            // result = arg1
            loadArg(c->arg1, "$t0", out);
            storeResult(c->result, "$t0", out);
        }
        else if (!strcmp(op, "+") || !strcmp(op, "-") ||
                 !strcmp(op, "*") || !strcmp(op, "/")) 
        {
            // result = arg1 op arg2
            loadArg(c->arg1, "$t1", out);  // left in $t1
            loadArg(c->arg2, "$t2", out);  // right in $t2

            if (!strcmp(op, "+")) {
                fprintf(out, "add $t0, $t1, $t2\n");
            } else if (!strcmp(op, "-")) {
                fprintf(out, "sub $t0, $t1, $t2\n");
            } else if (!strcmp(op, "*")) {
                fprintf(out, "mul $t0, $t1, $t2\n");
            } else if (!strcmp(op, "/")) {
                fprintf(out, "div $t1, $t2\n");
                fprintf(out, "mflo $t0\n");
            }
            // store into result
            storeResult(c->result, "$t0", out);
        }
        else if (!strcmp(op, "write")) {
            // write arg1 => load into $t0 => move $a0, $t0 => syscall
            loadArg(c->arg1, "$t0", out);
            fprintf(out, "move $a0, $t0\n");
            fprintf(out, "li $v0, 1\n");
            fprintf(out, "syscall\n");
            // newline
            fprintf(out, "li $a0, '\\n'\n");
            fprintf(out, "li $v0, 11\n");
            fprintf(out, "syscall\n");
        }
        else if (!strcmp(op, "load")) {
            // e.g. "t5 = load x" => t5 = x
            loadArg(c->arg1, "$t0", out);
            storeResult(c->result, "$t0", out);
        }
        else {
            // unhandled or label/goto, etc.
            fprintf(out, "# Unhandled TAC: %s %s %s %s\n",
                    op,
                    c->arg1 ? c->arg1 : "",
                    c->arg2 ? c->arg2 : "",
                    c->result ? c->result : "");
        }
    }

    // 4) Exit
    fprintf(out, "\n# exit\n");
    fprintf(out, "li $v0, 10\n");
    fprintf(out, "syscall\n");

    fclose(out);
    printf("[INFO] MIPS code generated in %s\n", outputFilename);
}
