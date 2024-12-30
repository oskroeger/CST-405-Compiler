#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h" // Ensure this includes the TAC definition and other necessary headers

// --------------------------------------------------------------------------
// 1) Data structures for register/spill management
// --------------------------------------------------------------------------

#define MAX_REGS 10  // Number of MIPS registers we'll use for temps ($t0..$t9)
#define MAX_TEMPS 200
#define MAX_LABELS 1000

typedef struct {
    char* tempName;          // e.g., "t_6"
    const char* assignedReg; // e.g., "$t3" or NULL if spilled
    int spilled;             // 0 = in register, 1 = spilled to memory
} TempBinding;

static TempBinding g_tempBindings[MAX_TEMPS];
static int g_numTempBindings = 0;

// Next free register index in [0..MAX_REGS-1]
static int g_nextRegIndex = 0;

// We'll store the names of user-defined variables, arrays, and spilled temps
// so we can declare them in .data later.
static char g_globalNames[500][64];
static int g_numGlobals = 0;

// To track array declarations and their sizes
typedef struct {
    char name[64];
    int size;
} ArrayInfo;

static ArrayInfo g_arrays[100];
static int g_numArrays = 0;

// Separate label counter for unique label generation
static int labelCounter = 0;

// Helper to see if a name is already in g_globalNames
static int isGlobalKnown(const char* name) {
    for (int i = 0; i < g_numGlobals; i++) {
        if (!strcmp(g_globalNames[i], name)) {
            return 1; // found
        }
    }
    return 0;
}

// Helper to see if an array is already known
static int isArrayKnown(const char* name) {
    for (int i = 0; i < g_numArrays; i++) {
        if (!strcmp(g_arrays[i].name, name)) {
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

// Add an array with its size
static void addArray(const char* name, int size) {
    if (!name) return;
    if (isArrayKnown(name)) return;
    strcpy(g_arrays[g_numArrays].name, name);
    g_arrays[g_numArrays].size = size;
    g_numArrays++;
}

// Return 1 if `str` is "t<number>" or "t_<number>", else 0
static int isTempName(const char* str) {
    if (!str) return 0;
    if (str[0] != 't') return 0;
    int i = 1;
    if (str[i] == '_') i++;
    if (!str[i]) return 0; // No digits after 't' or 't_'
    for (; str[i]; i++) {
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
// - If we still have free regs, assign one
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
        // Assign $t<g_nextRegIndex>
        static char regNameBuf[8];
        sprintf(regNameBuf, "$t%d", g_nextRegIndex);
        g_tempBindings[g_numTempBindings].assignedReg = strdup(regNameBuf);
        g_tempBindings[g_numTempBindings].spilled = 0;
        g_nextRegIndex++;

        fprintf(stderr, "DEBUG: Assigned %s to register %s\n", tempName, regNameBuf);
    } else {
        // Spill
        g_tempBindings[g_numTempBindings].assignedReg = NULL;
        g_tempBindings[g_numTempBindings].spilled = 1;

        // Mark that we need "var_t_X" in .data
        char labelBuf[64];
        sprintf(labelBuf, "var_%s", tempName);
        addGlobalName(labelBuf);

        fprintf(stderr, "DEBUG: Spilled %s to memory as %s\n", tempName, labelBuf);
    }

    g_numTempBindings++;
}

// Return the register if the temp is in a register, or NULL if spilled
static const char* getRegIfAny(const char* tempName) {
    TempBinding* b = lookupBinding(tempName);
    if (!b) return NULL;
    if (b->spilled) return NULL;
    return b->assignedReg; // e.g., "$t3"
}

// Return 1 if the given temp is spilled, else 0
static int isSpilled(const char* tempName) {
    TempBinding* b = lookupBinding(tempName);
    if (!b) return 0;
    return b->spilled;
}

// Return label name for a spilled temp, e.g., "var_t_6"
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
    // Non-temp => user variable or array
    // Check if it's an array by looking into g_arrays
    for (int i = 0; i < g_numArrays; i++) {
        if (!strcmp(g_arrays[i].name, var)) {
            // It's an array; handled separately
            return NULL;
        }
    }
    // It's a simple variable
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
        // User variable or immediate => no binding
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
    // 1) Ensure we allocate bindings for all temps and collect arrays
    for (TAC* c = tac; c; c = c->next) {
        if (c->arg1) ensureTempBinding(c->arg1);
        if (c->arg2) ensureTempBinding(c->arg2);
        if (c->result) ensureTempBinding(c->result);

        // Collect array declarations based on 'store' and 'load' operations
        if (c->operator) {
            if (!strcmp(c->operator, "store")) {
                // 'store' operations imply array usage
                // c->result is array name
                if (!isArrayKnown(c->result)) {
                    addArray(c->result, 5); // Adjust size as needed based on your declarations
                }
            }
            if (!strcmp(c->operator, "load")) {
                // 'load' operations imply array usage
                if (c->arg2) { // If arg2 exists, it's an array load
                    if (!isArrayKnown(c->arg1)) {
                        addArray(c->arg1, 5); // Adjust size as needed based on your declarations
                    }
                }
            }
        }
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
        // For +, -, *, /, ==, !=, <, <=, >, >= => check if arg1/arg2/result are user variables
        if (!strcmp(op, "+") || !strcmp(op, "-") ||
            !strcmp(op, "*") || !strcmp(op, "/") ||
            !strcmp(op, "==") || !strcmp(op, "!=") ||
            !strcmp(op, "<") || !strcmp(op, "<=") ||
            !strcmp(op, ">") || !strcmp(op, ">=")) {
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
    // Declare spilled temps and user variables
    for (int i = 0; i < g_numGlobals; i++) {
        fprintf(out, "%s: .word 0\n", g_globalNames[i]);
    }
    // Declare arrays
    for (int i = 0; i < g_numArrays; i++) {
        fprintf(out, "var_%s: .word ", g_arrays[i].name);
        for (int j = 0; j < g_arrays[i].size; j++) {
            fprintf(out, "0");
            if (j != g_arrays[i].size - 1) {
                fprintf(out, ", ");
            } else {
                fprintf(out, "\n");
            }
        }
    }
    fprintf(out, "\n.text\n");
}

// --------------------------------------------------------------------------
// 4) Load/store helpers
// --------------------------------------------------------------------------

static const char* loadArg(const char* arg, const char* scratch, FILE* out) {
    if (!arg) return scratch;

    // 1) Check if the argument is an integer literal
    char* endp;
    long val = strtol(arg, &endp, 10);
    if (*endp == '\0') {
        // Immediate value
        fprintf(out, "li %s, %ld\n", scratch, val);
        fprintf(stderr, "DEBUG: Loaded immediate %ld into %s\n", val, scratch);
        return scratch;
    }

    // 2) Check if the argument is a temporary variable
    if (isTempName(arg)) {
        const char* reg = getRegIfAny(arg);
        if (reg) {
            // Temporary is in a register; move it to the scratch register
            fprintf(out, "move %s, %s\n", scratch, reg);
            fprintf(stderr, "DEBUG: Moved %s from %s to %s\n", arg, reg, scratch);
            return scratch;
        } else if (isSpilled(arg)) {
            // Temporary is spilled to memory; load it into the scratch register using $s6
            char* spillLabel = getSpillLabel(arg);
            fprintf(out, "la $s6, %s\n", spillLabel);
            fprintf(out, "lw %s, 0($s6)\n", scratch);
            fprintf(stderr, "DEBUG: Loaded spilled %s into %s from %s\n", arg, scratch, spillLabel);
            free(spillLabel);
            return scratch;
        }
    }

    // 3) Handle user-defined variables or arrays
    if (isArrayKnown(arg)) {
        // Use $s7 for arrays
        fprintf(out, "la $s7, var_%s\n", arg);
        fprintf(stderr, "DEBUG: Loaded array %s base address into $s7 from var_%s\n", arg, arg);
        return "$s7"; // Return $s7 as the scratch register for array base
    } else {
        // Use $s6 for scalar variables
        char* g = makeGlobalName(arg);
        if (g) {
            fprintf(out, "la $s6, %s\n", g);
            fprintf(out, "lw %s, 0($s6)\n", scratch);
            fprintf(stderr, "DEBUG: Loaded %s into %s from %s\n", arg, scratch, g);
            free(g);
            return scratch;
        }
    }

    // Default case
    fprintf(stderr, "[ERROR] Unable to load argument: %s\n", arg);
    return "$zero";
}

static void storeResult(const char* dest, const char* srcReg, FILE* out) {
    if (!dest) return;

    if (isTempName(dest)) {
        const char* reg = getRegIfAny(dest);
        if (reg) {
            // Move srcReg into the assigned register
            fprintf(out, "move %s, %s\n", reg, srcReg);
            fprintf(stderr, "DEBUG: Moved %s into %s\n", srcReg, reg);
            return;
        } else if (isSpilled(dest)) {
            // Spilled, store to memory using $s6
            char* spillLabel = getSpillLabel(dest);
            fprintf(out, "la $s6, %s\n", spillLabel);
            fprintf(out, "sw %s, 0($s6)\n", srcReg);
            fprintf(stderr, "DEBUG: Stored %s into spilled %s from %s\n", srcReg, dest, spillLabel);
            free(spillLabel);
            return;
        }
    }

    if (isArrayKnown(dest)) {
        // Use $s7 for arrays
        fprintf(out, "sw %s, 0($s7)\n", srcReg);
        fprintf(stderr, "DEBUG: Stored %s into array %s from $s7\n", srcReg, dest);
    } else {
        // Use $s6 for scalar variables
        char* g = makeGlobalName(dest);
        if (g) {
            fprintf(out, "la $s6, %s\n", g);
            fprintf(out, "sw %s, 0($s6)\n", srcReg);
            fprintf(stderr, "DEBUG: Stored %s into %s from %s\n", srcReg, dest, g);
            free(g);
        }
    }
}

// --------------------------------------------------------------------------
// 5) Main code generator
// --------------------------------------------------------------------------

void generateMIPSFromTAC(TAC* tacHead, const char* outputFilename) {
    fprintf(stderr, "DEBUG: Using the FIXED code generator with array and while-loop support!\n");
    FILE* out = fopen(outputFilename, "w");
    if (!out) {
        fprintf(stderr, "[ERROR] Could not open %s\n", outputFilename);
        return;
    }

    // Reset global state
    g_numTempBindings = 0;
    g_nextRegIndex = 0;
    g_numGlobals = 0;
    g_numArrays = 0;
    labelCounter = 0; // Initialize label counter

    // 1) Collect global vars + do register assignment/spilling
    collectGlobalVars(tacHead, out);

    // 2) Emit .data
    // This is already handled in collectGlobalVars

    // 3) Start main
    fprintf(out, ".text\n");
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");

    // 4) Emit code for each TAC
    for (TAC* c = tacHead; c; c = c->next) {
        const char* op = c->operator ? c->operator : "";

        if (!strcmp(op, "=")) {
            // result = arg1
            loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
            storeResult(c->result, "$s0", out);      // Store from $s0 to result
        }
        else if (!strcmp(op, "+") || !strcmp(op, "-") ||
                 !strcmp(op, "*") || !strcmp(op, "/") ||
                 !strcmp(op, "==") || !strcmp(op, "!=") ||
                 !strcmp(op, "<") || !strcmp(op, "<=") ||
                 !strcmp(op, ">") || !strcmp(op, ">="))
        {
            // result = arg1 op arg2
            loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
            loadArg(c->arg2, "$s1", out);             // Load arg2 into $s1

            if (!strcmp(op, "==")) {
                // Handle equality
                // Compare $s0 and $s1, set $t9 to 1 if equal, else 0
                // Generate unique labels
                char label_true[64];
                char label_end[64];
                sprintf(label_true, "L_eq_true_%d", labelCounter++);
                sprintf(label_end, "L_eq_end_%d", labelCounter++);

                fprintf(out, "beq %s, %s, %s\n", "$s0", "$s1", label_true);
                fprintf(out, "li $t9, 0\n");
                fprintf(out, "j %s\n", label_end);
                fprintf(out, "%s:\n", label_true);
                fprintf(out, "li $t9, 1\n");
                fprintf(out, "%s:\n", label_end);

                // Store the result
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, "!=")) {
                // Handle inequality
                char label_true[64];
                char label_end[64];
                sprintf(label_true, "L_neq_true_%d", labelCounter++);
                sprintf(label_end, "L_neq_end_%d", labelCounter++);

                fprintf(out, "bne %s, %s, %s\n", "$s0", "$s1", label_true);
                fprintf(out, "li $t9, 0\n");
                fprintf(out, "j %s\n", label_end);
                fprintf(out, "%s:\n", label_true);
                fprintf(out, "li $t9, 1\n");
                fprintf(out, "%s:\n", label_end);

                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, "<")) {
                // Handle less than
                // Use 'slt $t9, $s0, $s1' sets $t9 to 1 if $s0 < $s1
                fprintf(out, "slt $t9, %s, %s\n", "$s0", "$s1\n");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, "<=")) {
                // Handle less than or equal
                // slt $t9, $s1, $s0 -> $t9 = 1 if $s1 < $s0
                // xori $t9, $t9, 1 -> $t9 = 0 if $s1 < $s0 else 1
                fprintf(out, "slt $t9, %s, %s\n", "$s1", "$s0");
                fprintf(out, "xori $t9, $t9, 1\n");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, ">")) {
                // Handle greater than
                // slt $t9, $s1, $s0
                fprintf(out, "slt $t9, %s, %s\n", "$s1", "$s0");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, ">=")) {
                // Handle greater than or equal
                // slt $t9, $s0, $s1 -> $t9 = 1 if $s0 < $s1
                // xori $t9, $t9, 1 -> $t9 = 0 if $s0 < $s1 else 1
                fprintf(out, "slt $t9, %s, %s\n", "$s0", "$s1");
                fprintf(out, "xori $t9, $t9, 1\n");
                storeResult(c->result, "$t9", out);
            }
            else {
                // Handle arithmetic operators: +, -, *, /
                // Perform the operation using $t9 as a temporary
                if (!strcmp(op, "+")) {
                    fprintf(out, "add $t9, $s0, $s1\n");
                } else if (!strcmp(op, "-")) {
                    fprintf(out, "sub $t9, $s0, $s1\n");
                } else if (!strcmp(op, "*")) {
                    fprintf(out, "mul $t9, $s0, $s1\n");
                } else if (!strcmp(op, "/")) {
                    fprintf(out, "div %s, %s\n", "$s0", "$s1");
                    fprintf(out, "mflo $t9\n");
                }

                // Store the result
                storeResult(c->result, "$t9", out);
            }
        }
        else if (!strcmp(op, "write")) {
            // write arg1 => load into $a0 => syscall
            loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
            fprintf(out, "move $a0, $s0\n");          // Move to $a0
            fprintf(out, "li $v0, 1\n");              // Syscall code for print integer
            fprintf(out, "syscall\n");
            // Print newline
            fprintf(out, "li $a0, 10\n");             // ASCII for newline is 10
            fprintf(out, "li $v0, 11\n");             // Syscall code for print character
            fprintf(out, "syscall\n");
        }
        else if (!strcmp(op, "load")) {
            if (c->arg2) {
                // Array access: result = load arr[index]
                // 1. Load base address of array into $s7
                loadArg(c->arg1, "$s7", out);           // Load base address into $s7

                // 2. Load index into $s0
                loadArg(c->arg2, "$s0", out);

                // 3. Multiply index by 4: sll $t0, $s0, 2
                fprintf(out, "sll $t0, $s0, 2\n");

                // 4. Add base address to get effective address: add $t0, $s7, $t0
                fprintf(out, "add $t0, $s7, $t0\n");

                // 5. Load word from address: lw $t9, 0($t0)
                fprintf(out, "lw $t9, 0($t0)\n");

                // 6. Store into result: z = $t9
                storeResult(c->result, "$t9", out);
            }
            else {
                // Simple load: result = load var
                loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
                storeResult(c->result, "$s0", out);      // Store from $s0 to result
            }
        }
        else if (!strcmp(op, "store")) {
            // store arg1 into result[arg2]
            // 1. Load base address of array into $s7
            loadArg(c->result, "$s7", out);             // Load base address into $s7

            // 2. Load index into $s0
            loadArg(c->arg2, "$s0", out);

            // 3. Multiply index by 4: sll $t0, $s0, 2
            fprintf(out, "sll $t0, $s0, 2\n");

            // 4. Add base address to get effective address: add $t0, $s7, $t0
            fprintf(out, "add $t0, $s7, $t0\n");

            // 5. Load value to store into $s1
            loadArg(c->arg1, "$s1", out);

            // 6. Store word into address: sw $s1, 0($t0)
            fprintf(out, "sw $s1, 0($t0)\n");
        }
        else {
            // Handle other operators or unhandled TAC
            if (!strcmp(op, "ifFalse")) {
                // ifFalse cond goto label
                loadArg(c->arg1, "$s0", out);             // Load condition into $s0
                fprintf(out, "beq $s0, $zero, %s\n", c->result);
            }
            else if (!strcmp(op, "if")) {
                // if cond goto label
                loadArg(c->arg1, "$s0", out);             // Load condition into $s0
                fprintf(out, "bne $s0, $zero, %s\n", c->result);
            }
            else if (!strcmp(op, "goto")) {
                // Unconditional jump
                fprintf(out, "j %s\n", c->result);
            }
            else if (!strcmp(op, "label")) {
                // Label definition
                fprintf(out, "%s:\n", c->result);
            }
            else if (!strcmp(op, "call")) {
                // Function call
                fprintf(out, "jal %s\n", c->arg1);
                if (c->result) {
                    // Move return value from $v0 to result
                    const char* r = getRegIfAny(c->result);
                    if (r) {
                        fprintf(out, "move %s, $v0\n", r);
                    } else if (isSpilled(c->result)) {
                        storeResult(c->result, "$v0", out);
                    } else {
                        // Should not happen
                        fprintf(stderr, "[ERROR] Unknown binding for %s\n", c->result);
                    }
                }
            }
            else if (!strcmp(op, "return")) {
                // Return statement
                if (c->arg1) {
                    loadArg(c->arg1, "$v0", out);         // Load return value into $v0
                }
                fprintf(out, "jr $ra\n");
            }
            else if (!strcmp(op, "param")) {
                // Handle function parameters
                // Assuming parameters are passed via $a0..$a3
                static int paramCount = 0;
                if (paramCount < 4) {
                    loadArg(c->arg1, "$s0", out);         // Load parameter into $s0
                    fprintf(out, "move $a%d, $s0\n", paramCount);
                } else {
                    // Handle additional parameters via stack
                    fprintf(out, "# TODO: Handle additional parameters via stack\n");
                }
                paramCount++;
            }
            else {
                // Unhandled or other TAC operations
                fprintf(out, "# Unhandled TAC: %s %s %s %s\n",
                        op,
                        c->arg1 ? c->arg1 : "",
                        c->arg2 ? c->arg2 : "",
                        c->result ? c->result : "");
            }
        }
    }

    // 5) Exit the program
    fprintf(out, "li $v0, 10\n");
    fprintf(out, "syscall\n");

    fclose(out);
}
