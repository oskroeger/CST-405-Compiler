#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h" // Ensure this includes the TAC definition and other necessary headers

// --------------------------------------------------------------------------
// 1) Data structures for register/spill management
// --------------------------------------------------------------------------

#define MAX_REGS 10  // Number of MIPS registers we'll use for temps ($t0..$t9)
#define MAX_FLOAT_REGS 10  // Number of MIPS registers we'll use for float temps ($f0..$f9)
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
static int g_nextFloatRegIndex = 0;

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

// Current function tracking
static char currentFunctionEndLabel[64] = "";

// --------------------------------------------------------------------------

// A global or static structure to track float constants
typedef struct {
    float value;
    char label[32];  // e.g. "LC_flt_0"
} FloatConst;

static FloatConst g_floatConsts[100]; 
static int g_numFloatConsts = 0;

// Return label name (e.g. "LC_flt_5") for the float value
const char* getFloatLabel(float val) {
    // Check if val already in g_floatConsts
    for (int i = 0; i < g_numFloatConsts; i++) {
        if (g_floatConsts[i].value == val) {
            return g_floatConsts[i].label;
        }
    }
    // If not found, create new
    sprintf(g_floatConsts[g_numFloatConsts].label, "LC_flt_%d", g_numFloatConsts);
    g_floatConsts[g_numFloatConsts].value = val;
    g_numFloatConsts++;
    return g_floatConsts[g_numFloatConsts - 1].label;
}

static void printAllFloatConsts(FILE* out) {
    fprintf(out, ".data\n");
    // Print out each float constant
    for (int i = 0; i < g_numFloatConsts; i++) {
        fprintf(out, "%s: .float %f\n", 
                   g_floatConsts[i].label, 
                   g_floatConsts[i].value);
    }
    fprintf(out, "\n.text\n");
}

// --------------------------------------------------------------------------

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

// Return 1 if `str` is "t<number>" or "t_<number>", , 2 if its float or else 0
static int isTempName(const char* str) {
    if (!str) return 0;
    // 1) Check if 'f' for float
    if (strncmp(str, "f", 2) == 0) {
        int i = 2;
        if (str[i] == '_') i++;
        if (!str[i]) return 0; // no digits
        for (; str[i]; i++) {
            if (str[i] < '0' || str[i] > '9') return 0;
        }
        return 2;  // float
    }
    // 2) Else check if 't'
    if (str[0] == 't') {
        int i = 1;
        if (str[i] == '_') i++;
        if (!str[i]) return 0;
        for (; str[i]; i++) {
            if (str[i] < '0' || str[i] > '9') return 0;
        }
        return 1; // int
    }
    return 0;
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

    // Create a new binding struct
    g_tempBindings[g_numTempBindings].tempName   = strdup(tempName);
    g_tempBindings[g_numTempBindings].assignedReg = NULL;
    g_tempBindings[g_numTempBindings].spilled    = 0;

    // Detect if int or float
    int type = isTempName(tempName);  // from the unified function above
    //   or if you used separate isTempName/isFloatTempName, do:
    //   if (isFloatTempName(tempName)) { ... } else if (isTempName(tempName)) { ... }

    if (type == 2) {
        // This is a float temp => try to assign $fX
        if (g_nextFloatRegIndex < MAX_FLOAT_REGS) {
            // e.g. assign $f0, $f1, ...
            static char regNameBuf[8];
            sprintf(regNameBuf, "$f%d", g_nextFloatRegIndex);
            g_tempBindings[g_numTempBindings].assignedReg = strdup(regNameBuf);
            g_tempBindings[g_numTempBindings].spilled     = 0;
            g_nextFloatRegIndex++;

            fprintf(stderr, "DEBUG: Assigned %s to float reg %s\n", tempName, regNameBuf);
        } else {
            // Spill float
            g_tempBindings[g_numTempBindings].assignedReg = NULL;
            g_tempBindings[g_numTempBindings].spilled     = 1;
            char labelBuf[64];
            sprintf(labelBuf, "var_%s", tempName);
            addGlobalName(labelBuf);
            fprintf(stderr, "DEBUG: Spilled float %s to memory as %s\n", tempName, labelBuf);
        }
    } 
    else if (type == 1) {
        // This is an int temp => same logic as before
        if (g_nextRegIndex < MAX_REGS) {
            static char regNameBuf[8];
            sprintf(regNameBuf, "$t%d", g_nextRegIndex);
            g_tempBindings[g_numTempBindings].assignedReg = strdup(regNameBuf);
            g_tempBindings[g_numTempBindings].spilled     = 0;
            g_nextRegIndex++;

            fprintf(stderr, "DEBUG: Assigned %s to int reg %s\n", tempName, regNameBuf);
        } else {
            // Spill int
            g_tempBindings[g_numTempBindings].assignedReg = NULL;
            g_tempBindings[g_numTempBindings].spilled     = 1;
            char labelBuf[64];
            sprintf(labelBuf, "var_%s", tempName);
            addGlobalName(labelBuf);

            fprintf(stderr, "DEBUG: Spilled int %s to memory as %s\n", tempName, labelBuf);
        }
    } 
    else {
        // If it's neither t_# nor ft_# => do something fallback or error
        // But presumably it won't get here if you only call allocateBinding for recognized temps
        fprintf(stderr, "[ERROR] allocateBinding called with unknown tempName: %s\n", tempName);
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
            if (!strcmp(c->operator, "call")) {
                // Functions may have variables needing to be global or on stack
                // Additional handling if necessary
            }
        }
    }

    // 2) For each instruction, if there's a user-defined var, add to .data
    for (TAC* c = tac; c; c = c->next) {
        const char* op = c->operator ? c->operator : "";

        // For assignment or write/load, etc.
        if ((!strcmp(op, "=") || !strcmp(op, "f=")) && c->result) {
            if (!isTempName(c->result)) {
                // **NEW LOGIC**: Exclude parameters from global variables
                int is_param = 0;
                if (currentFunctionEndLabel[0] != '\0') {
                    // **UPDATED LOGIC**: Exclude 'a' and 'b' for any function
                    if (strcmp(c->result, "a") == 0 || strcmp(c->result, "b") == 0) {
                        is_param = 1;
                    }
                }
                if (!is_param) {
                    char* g = makeGlobalName(c->result);
                    if (g) { addGlobalName(g); free(g); }
                }
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
        } else if (!strcmp(op, "f+") || !strcmp(op, "f-") ||
            !strcmp(op, "f*") || !strcmp(op, "f/") ||
            !strcmp(op, "f==") || !strcmp(op, "f!=") ||
            !strcmp(op, "f<") || !strcmp(op, "f<=") ||
            !strcmp(op, "f>") || !strcmp(op, "f>=")) {
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
        // Handle return statements
        if (!strcmp(op, "return") && c->arg1) {
            if (!isTempName(c->arg1)) {
                char* g = makeGlobalName(c->arg1);
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
        fprintf(out, "    li %s, %ld\n", scratch, val);
        fprintf(stderr, "DEBUG: Loaded immediate %ld into %s\n", val, scratch);
        return scratch;
    }

    // 2) Check if the argument is a temporary variable
    if (isTempName(arg)) {
        const char* reg = getRegIfAny(arg);
        if (reg) {
            // Temporary is in a register; move it to the scratch register
            fprintf(out, "    move %s, %s\n", scratch, reg);
            fprintf(stderr, "DEBUG: Moved %s from %s to %s\n", arg, reg, scratch);
            return scratch;
        } else if (isSpilled(arg)) {
            // Temporary is spilled to memory; load it into the scratch register using $s6
            char* spillLabel = getSpillLabel(arg);
            fprintf(out, "    lw %s, %s\n", scratch, spillLabel);
            fprintf(stderr, "DEBUG: Loaded spilled %s into %s from %s\n", arg, scratch, spillLabel);
            free(spillLabel);
            return scratch;
        }
    }

    // 3) Handle user-defined variables or arrays
    if (isArrayKnown(arg)) {
        // Use $s7 for arrays
        fprintf(out, "    la $s7, var_%s\n", arg);
        fprintf(stderr, "DEBUG: Loaded array %s base address into $s7 from var_%s\n", arg, arg);
        return "$s7"; // Return $s7 as the scratch register for array base
    } else {
        // Use $s6 for scalar variables
        char* g = makeGlobalName(arg);
        if (g) {
            // **NEW LOGIC**: Map 'a' and 'b' to $a0 and $a1 for any function
            if (currentFunctionEndLabel[0] != '\0') {
                // Assume all functions have parameters named 'a' and 'b'
                if (strcmp(arg, "a") == 0) {
                    fprintf(out, "    move %s, $a0\n", scratch);
                    fprintf(stderr, "DEBUG: Moved $a0 to %s\n", scratch);
                    return scratch;
                }
                if (strcmp(arg, "b") == 0) {
                    fprintf(out, "    move %s, $a1\n", scratch);
                    fprintf(stderr, "DEBUG: Moved $a1 to %s\n", scratch);
                    return scratch;
                }
            }

            // If not a parameter, load from global variable
            fprintf(out, "    lw %s, %s\n", scratch, g);
            fprintf(stderr, "DEBUG: Loaded %s into %s from %s\n", arg, scratch, g);
            free(g);
            return scratch;
        }
    }

    // Default case
    fprintf(stderr, "[ERROR] Unable to load argument: %s\n", arg);
    return "$zero";
}

static const char* loadFloatArg(const char* arg, const char* fScratch, FILE* out) {
    if (!arg) return fScratch;  // if there's no argument, just return the float register

    // 1) Check if the argument is a float literal, e.g. "2.3"
    char* endp;
    float val = strtof(arg, &endp);
    if (*endp == '\0') {
        // It's a numeric float literal
        //
        // (A) Create or reuse a label in .data for this float (like LC_flt_0).
        // (B) la $t0, LC_flt_0
        // (C) l.s $fScratch, 0($t0)
        const char* label = getFloatLabel(val); // e.g. "LC_flt_0"
        fprintf(out, "    la   $t0, %s\n", label);
        fprintf(out, "    l.s  %s, 0($t0)\n", fScratch);
        return fScratch;
    }

    // 2) If the argument is a float *temp* (like f_5 or ft_5)
    if (isTempName(arg) == 2) {
        const char* reg = getRegIfAny(arg);   // see if in a dedicated $fX register
        if (reg) {
            // If you store float temps directly in a float register, do something like "mov.s"
            // MIPS doesn’t have a direct 'move' for floats but you can do "mov.s $fScratch, $f2"
            // or "cvt.s.s" in some toolchains.  Usually, it’s "mov.s $fScratch, <reg>".
            // For SPIM or MARS, there's a pseudo-instruction "mov.s" for registers:
            fprintf(out, "    mov.s %s, %s\n", fScratch, reg);
            return fScratch;
        } 
        else if (isSpilled(arg)) {
            // If spilled, we stored it in memory (var_f_5). So do l.s:
            char* spillLabel = getSpillLabel(arg);  // e.g. "var_f_5"
            fprintf(out, "    l.s %s, %s\n", fScratch, spillLabel);
            free(spillLabel);
            return fScratch;
        }
    }

    // 3) If it's a float array reference, you'd do something like:
    if (isArrayKnown(arg)) {
        // la $s7, var_<arg>  (or however you handle float arrays)
        fprintf(out, "    la $s7, var_%s\n", arg);
        return "$s7"; 
    }

    // 4) If it's a user-defined float variable (e.g., "x" that the symbol table says is float)
    //    -> we do "l.s fScratch, var_x"
    char* g = makeGlobalName(arg);  // e.g. "var_x"
    if (g) {
        fprintf(out, "    l.s %s, %s\n", fScratch, g);
        free(g);
        return fScratch;
    }

    // If all else fails:
    fprintf(stderr, "[ERROR] loadFloatArg: unable to load '%s'\n", arg);
    return fScratch;
}


/*static const char* loadFloatArg(const char* arg, const char* fReg, FILE* out) {
    if (!arg) return fReg;

    // 1) Check if `arg` is a float literal
    //    We can see if it has a '.' or try to parse as float, etc.
    //    Or if your IR always sets "t_XXX" or "ft_XXX" for temps, you can handle that logic differently.
    
    char* endp;
    float floatVal = strtof(arg, &endp);
    if (*endp == '\0') {
        // This means arg was purely numeric => float literal
        // We might store it in a temp data label or load immediate
        // MIPS doesn't have a "li.s $fX, immediate" so we do:
        
        //  A) create a label in .data for this literal
        //  B) load it with la + l.s
        static int floatLiteralCount = 0;
        char labelName[32];
        sprintf(labelName, "LC_flt_%d", floatLiteralCount++);
        fprintf(out, ".data\n");
        fprintf(out, "%s: .float %f\n", labelName, floatVal);
        fprintf(out, ".text\n");

        // la $t0, labelName
        // l.s $fReg, 0($t0)
        fprintf(out, "    la $t0, %s\n", labelName);
        fprintf(out, "    l.s %s, 0($t0)\n", fReg);
        return fReg;


    }

    // 2) If arg is a temp like t_5 or ft_5
    //    We can see if it's in a float register or spilled
    if (isTempName(arg)) {
        // If we do no separate float vs int registers, we might need to see if it's "ft_"
        // But let's assume we just do "t_5" => we track in a separate binding or logic.
        // We'll do a simple approach:
        // See if it's spilled => load with l.s
        // else if in register => use 'mov.s' (like "move" but for floats)...

        // For simplicity in your code, you might say "We always spill floats" or something,
        // but let's do a naive version:
        
        // There's no direct float binding logic in your code yet,
        // so let's assume we store float temps in memory (spilled).
        
        if (isSpilled(arg)) {
            char* spillLabel = getSpillLabel(arg);  // e.g. var_t_5
            fprintf(out, "    l.s %s, %s\n", fReg, spillLabel);
            free(spillLabel);
            return fReg;
        } else {
            // If not spilled, we might need a separate approach
            // (like if you introduced separate $f registers in your register manager).
            // For now, let's fallback:
            fprintf(stderr, "[ERROR] float temp in register not fully handled\n");
            fprintf(out, "    # TODO: handle float temp in register\n");
            return fReg;
        }
    }

    // 3) If it's a user-defined float variable:
    //    We'll do something similar to your loadArg code, but using l.s
    if (isArrayKnown(arg)) {
        // For arrays, you'd do la + offset, then l.s. 
        // We'll skip the details here, as in your code you handle it differently.
        fprintf(stderr, "[ERROR] float array not handled yet.\n");
        return fReg;
    } else {
        // Normal scalar variable => "var_x"
        char* g = makeGlobalName(arg);
        if (g) {
            // e.g. "var_x"
            // Use l.s to load the float
            fprintf(out, "    l.s %s, %s\n", fReg, g);
            free(g);
            return fReg;
        }
    }

    // If all else fails:
    fprintf(stderr, "[ERROR] loadFloatArg: could not handle arg '%s'\n", arg);
    return fReg;
} */


static void storeResult(const char* dest, const char* srcReg, FILE* out) {
    if (!dest) return;

    if (isTempName(dest)) {
        const char* reg = getRegIfAny(dest);
        if (reg) {
            // Move srcReg into the assigned register
            fprintf(out, "    move %s, %s\n", reg, srcReg);
            fprintf(stderr, "DEBUG: Moved %s into %s\n", srcReg, reg);
            return;
        } else if (isSpilled(dest)) {
            // Spilled, store to memory
            char* spillLabel = getSpillLabel(dest);
            fprintf(out, "    sw %s, %s\n", srcReg, spillLabel);
            fprintf(stderr, "DEBUG: Stored %s into spilled %s from %s\n", srcReg, dest, spillLabel);
            free(spillLabel);
            return;
        }
    }

    if (isArrayKnown(dest)) {
        // Use $s7 for arrays
        fprintf(out, "    sw %s, 0($s7)\n", srcReg);
        fprintf(stderr, "DEBUG: Stored %s into array %s from $s7\n", srcReg, dest);
    } else {
        // Use $s6 for scalar variables
        char* g = makeGlobalName(dest);
        if (g) {
            fprintf(out, "    sw %s, %s\n", srcReg, g);
            fprintf(stderr, "DEBUG: Stored %s into %s from %s\n", srcReg, dest, g);
            free(g);
        }
    }
}

static void storeFloatResult(const char* dest, const char* fReg, FILE* out) {
    if (!dest) return;

    if (isTempName(dest)) {
        // If spilled, store with s.s
        if (isSpilled(dest)) {
            char* spillLabel = getSpillLabel(dest);
            fprintf(out, "    s.s %s, %s\n", fReg, spillLabel);
            fprintf(stderr, "DEBUG: Stored float %s into spilled %s\n", fReg, dest);
            free(spillLabel);
        } else {
            // If we had a dedicated float register assignment, we'd do something like:
            fprintf(stderr, "[ERROR] storeFloatResult for in-register float temps not implemented.\n");
        }
        return;
    }

    // If it's a user variable:
    if (isArrayKnown(dest)) {
        // array store => s.s ...
        fprintf(stderr, "[ERROR] storeFloatResult for array not implemented.\n");
    } else {
        char* g = makeGlobalName(dest);
        if (g) {
            fprintf(out, "    s.s %s, %s\n", fReg, g);
            free(g);
        } else {
            fprintf(stderr, "[ERROR] storeFloatResult: cannot handle '%s'\n", dest);
        }
    }
}


// --------------------------------------------------------------------------
// 5) Function Prologue and Epilogue Generators
// --------------------------------------------------------------------------

// Generate function prologue
static void generateFunctionPrologue(const char* funcLabel, FILE* out) {
    fprintf(out, "%s:\n", funcLabel);
    // Prologue: Adjust stack and save $ra and $fp
    fprintf(out, "    addi $sp, $sp, -8\n");   // Allocate stack space
    fprintf(out, "    sw $ra, 4($sp)\n");      // Save return address
    fprintf(out, "    sw $fp, 0($sp)\n");      // Save frame pointer
    fprintf(out, "    move $fp, $sp\n");       // Set frame pointer
    // Set the current function's end label for return statements
    sprintf(currentFunctionEndLabel, "endfunc_%s", funcLabel + 5); // Remove 'func_' prefix
}

// Generate function epilogue
static void generateFunctionEpilogue(const char* funcLabel, FILE* out) {
    // Epilogue label
    fprintf(out, "%s:\n", funcLabel);
    // Epilogue: Restore $ra and $fp, deallocate stack space
    fprintf(out, "    move $sp, $fp\n");       // Restore stack pointer
    fprintf(out, "    lw $fp, 0($sp)\n");      // Restore frame pointer
    fprintf(out, "    lw $ra, 4($sp)\n");      // Restore return address
    fprintf(out, "    addi $sp, $sp, 8\n");    // Deallocate stack space
    fprintf(out, "    jr $ra\n");              // Return to caller
}

// --------------------------------------------------------------------------
// 6) Main code generator
// --------------------------------------------------------------------------

// Helper method to determing if an expression is a float expression
static int isEitherFloatTemp(const char* a, const char* b) {
    // Return 1 if either a or b is a float temp
    // We'll rely on your isTempName returning 2 for float
    if (a && isTempName(a) == 2) return 1;
    if (b && isTempName(b) == 2) return 1;
    return 0;
}


void generateMIPSFromTAC(TAC* tacHead, const char* outputFilename) {
    fprintf(stderr, "DEBUG: Using the FIXED code generator with array, while-loop, and function support!\n");
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
    currentFunctionEndLabel[0] = '\0'; // Reset current function label

    // 1) Collect global vars + do register assignment/spilling
    collectGlobalVars(tacHead, out);

    // 2) Emit .data
    // Already handled in collectGlobalVars

    // 3) Initialize flags for tracking
    int in_function = 0;      // Flag to track if we're inside a function
    int main_emitted = 0;     // Flag to ensure 'main:' is emitted only once
    int paramCount = 0;       // Counter for parameters

    // 4) Iterate through TAC and emit code sequentially
    for (TAC* c = tacHead; c; c = c->next) {
        const char* op = c->operator ? c->operator : "";

        // Check for function start
        if (!strcmp(op, "label")) {
            if (strncmp(c->result, "func_", 5) == 0) {
                // Function label detected
                generateFunctionPrologue(c->result, out);
                in_function = 1;
                paramCount = 0; // Reset parameter count for new function
                continue;
            }
        }

        // Check for function end
        if (!strcmp(op, "label")) {
            if (strncmp(c->result, "endfunc_", 8) == 0) {
                // Function epilogue label detected
                generateFunctionEpilogue(c->result, out);
                in_function = 0;
                currentFunctionEndLabel[0] = '\0'; // Reset after function end
                continue;
            }
        }

        // If not inside a function and 'main:' is not yet emitted, emit 'main:'
        if (!in_function && !main_emitted) {
            fprintf(out, "main:\n");
            main_emitted = 1;
        }
        // if op is = and the any of teh args containg f like this f_2 f= f_3 f+ f_4
        else if ( (!strcmp(op, "=") && isEitherFloatTemp(c->arg1, c->result)) || !strcmp(op, "f=") ) {
            // result = arg1, but both are float
            loadFloatArg(c->arg1, "$f0", out);     // load the float source into $f0
            storeFloatResult(c->result, "$f0", out); // store into result
        }

        // Emit code based on operation
        else if (!strcmp(op, "=")) {
            // result = arg1
            loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
            storeResult(c->result, "$s0", out);      // Store from $s0 to result
        }


        else if (!strcmp(op, "f+") || !strcmp(op, "f-") ||
         !strcmp(op, "f*") || !strcmp(op, "f/") ||
         !strcmp(op, "f==") || !strcmp(op, "f!=") ||
         !strcmp(op, "f<") || !strcmp(op, "f<=") ||
         !strcmp(op, "f>") || !strcmp(op, "f>="))
        {
            // 1) Load float arguments into registers, e.g. $f0 and $f1
            loadFloatArg(c->arg1, "$f0", out);
            loadFloatArg(c->arg2, "$f1", out);

            // 2) If it's one of the arithmetic ops (f+, f-, f*, f/), do add.s etc.
            if (!strcmp(op, "f+")) {
                fprintf(out, "    add.s $f2, $f0, $f1\n");
                storeFloatResult(c->result, "$f2", out);
            }
            else if (!strcmp(op, "f-")) {
                fprintf(out, "    sub.s $f2, $f0, $f1\n");
                storeFloatResult(c->result, "$f2", out);
            }
            else if (!strcmp(op, "f*")) {
                fprintf(out, "    mul.s $f2, $f0, $f1\n");
                storeFloatResult(c->result, "$f2", out);
            }
            else if (!strcmp(op, "f/")) {
                fprintf(out, "    div.s $f2, $f0, $f1\n");
                storeFloatResult(c->result, "$f2", out);
            }
            else {
                // 3) If it's a comparison (f==, f<, etc.), use MIPS float compares
                //    e.g., c.eq.s, c.lt.s, c.le.s, etc. plus bc1t / bc1f.
                //    Then store 1 or 0 in an integer register to place in the result.
                // Example: f== => c.eq.s $f0, $f1
                // Then use bc1t / bc1f to set $t9 accordingly.
                
                if (!strcmp(op, "f==")) {
                    // We'll do something like:
                    // c.eq.s $f0, $f1
                    // bc1t label_true
                    // li $t9, 0
                    // j label_end
                    // label_true:
                    // li $t9, 1
                    // label_end:
                    // storeResult(...)
                    
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_feq_true_%d", labelCounter++);
                    sprintf(label_end,  "L_feq_end_%d",  labelCounter++);

                    // Compare
                    fprintf(out, "    c.eq.s $f0, $f1\n");
                    // Branch if c1 condition is true
                    fprintf(out, "    bc1t %s\n", label_true);
                    // Else false
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 1\n");
                    fprintf(out, "%s:\n", label_end);

                    // store $t9 in the result
                    storeResult(c->result, "$t9", out);
                }
                else if (!strcmp(op, "f!=")) {
                    // c.eq.s => bc1t => means equal => so if bc1t => eq => we want not eq => do inverse logic
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_fneq_true_%d", labelCounter++);
                    sprintf(label_end,  "L_fneq_end_%d",  labelCounter++);

                    fprintf(out, "    c.eq.s $f0, $f1\n");
                    // If they're equal => c1 is set => we skip => so if bc1t => they're equal => not !=
                    fprintf(out, "    bc1t %s\n", label_true);
                    // If we didn't branch => they're not equal => set 1
                    fprintf(out, "    li $t9, 1\n");
                    // Jump to end
                    fprintf(out, "    j %s\n", label_end);
                    // If bc1t => eq => then 0
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "%s:\n", label_end);

                    storeResult(c->result, "$t9", out);
                }
                else if (!strcmp(op, "f<")) {
                    // c.lt.s => bc1t => means $f0 < $f1
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_flt_true_%d", labelCounter++);
                    sprintf(label_end,  "L_flt_end_%d",  labelCounter++);

                    fprintf(out, "    c.lt.s $f0, $f1\n");
                    fprintf(out, "    bc1t %s\n", label_true);
                    // else
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 1\n");
                    fprintf(out, "%s:\n", label_end);

                    storeResult(c->result, "$t9", out);
                }
                else if (!strcmp(op, "f<=")) {
                    // c.le.s => bc1t => means $f0 <= $f1
                    // Similar pattern
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_fle_true_%d", labelCounter++);
                    sprintf(label_end,  "L_fle_end_%d",  labelCounter++);

                    fprintf(out, "    c.le.s $f0, $f1\n");
                    fprintf(out, "    bc1t %s\n", label_true);
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 1\n");
                    fprintf(out, "%s:\n", label_end);

                    storeResult(c->result, "$t9", out);
                }
                else if (!strcmp(op, "f>")) {
                    // "f>" => c.lt.s $f1, $f0
                    // or c.le.s and invert, etc.
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_fgt_true_%d", labelCounter++);
                    sprintf(label_end,  "L_fgt_end_%d",  labelCounter++);

                    // We'll do c.lt.s $f1, $f0 => bc1t => means $f1 < $f0 => so $f0 > $f1
                    fprintf(out, "    c.lt.s $f1, $f0\n");
                    fprintf(out, "    bc1t %s\n", label_true);
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 1\n");
                    fprintf(out, "%s:\n", label_end);

                    storeResult(c->result, "$t9", out);
                }
                else if (!strcmp(op, "f>=")) {
                    // "f>=" => c.le.s $f1, $f0
                    char label_true[64], label_end[64];
                    sprintf(label_true, "L_fge_true_%d", labelCounter++);
                    sprintf(label_end,  "L_fge_end_%d",  labelCounter++);

                    fprintf(out, "    c.le.s $f1, $f0\n");
                    fprintf(out, "    bc1t %s\n", label_true);
                    fprintf(out, "    li $t9, 0\n");
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    li $t9, 1\n");
                    fprintf(out, "%s:\n", label_end);

                    storeResult(c->result, "$t9", out);
                }
            }
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

                fprintf(out, "    beq %s, %s, %s\n", "$s0", "$s1", label_true);
                fprintf(out, "    li $t9, 0\n");
                fprintf(out, "    j %s\n", label_end);
                fprintf(out, "%s:\n", label_true);
                fprintf(out, "    li $t9, 1\n");
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

                fprintf(out, "    bne %s, %s, %s\n", "$s0", "$s1", label_true);
                fprintf(out, "    li $t9, 0\n");
                fprintf(out, "    j %s\n", label_end);
                fprintf(out, "%s:\n", label_true);
                fprintf(out, "    li $t9, 1\n");
                fprintf(out, "%s:\n", label_end);

                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, "<")) {
                // Handle less than
                // Use 'slt $t9, $s0, $s1' sets $t9 to 1 if $s0 < $s1
                fprintf(out, "    slt $t9, %s, %s\n", "$s0", "$s1");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, "<=")) {
                // Handle less than or equal
                // slt $t9, $s1, $s0 -> $t9 = 1 if $s1 < $s0
                // xori $t9, $t9, 1 -> $t9 = 0 if $s1 < $s0 else 1
                fprintf(out, "    slt $t9, %s, %s\n", "$s1", "$s0");
                fprintf(out, "    xori $t9, $t9, 1\n");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, ">")) {
                // Handle greater than
                // slt $t9, $s1, $s0
                fprintf(out, "    slt $t9, %s, %s\n", "$s1", "$s0");
                storeResult(c->result, "$t9", out);
            }
            else if (!strcmp(op, ">=")) {
                // Handle greater than or equal
                // slt $t9, $s0, $s1 -> $t9 = 1 if $s0 < $s1
                // xori $t9, $t9, 1 -> $t9 = 0 if $s0 < $s1 else 1
                fprintf(out, "    slt $t9, %s, %s\n", "$s0", "$s1");
                fprintf(out, "    xori $t9, $t9, 1\n");
                storeResult(c->result, "$t9", out);
            }
            else {
                // Handle arithmetic operators: +, -, *, /
                // Perform the operation using $t9 as a temporary
                if (!strcmp(op, "+")) {
                    fprintf(out, "    add $t9, %s, %s\n", "$s0", "$s1");
                } else if (!strcmp(op, "-")) {
                    fprintf(out, "    sub $t9, %s, %s\n", "$s0", "$s1");
                } else if (!strcmp(op, "*")) {
                    fprintf(out, "    mul $t9, %s, %s\n", "$s0", "$s1");
                } else if (!strcmp(op, "/")) {
                    fprintf(out, "    div %s, %s\n", "$s0", "$s1");
                    fprintf(out, "    mflo $t9\n");
                }

                // Store the result
                storeResult(c->result, "$t9", out);
            }
        }
        else if (!strcmp(op, "write")) {
            // write arg1 => load into $a0 => syscall
            loadArg(c->arg1, "$s0", out);             // Load arg1 into $s0
            fprintf(out, "    move $a0, $s0\n");          // Move to $a0
            fprintf(out, "    li $v0, 1\n");              // Syscall code for print integer
            fprintf(out, "    syscall\n");
            // Print newline
            fprintf(out, "    li $a0, 10\n");             // ASCII for newline is 10
            fprintf(out, "    li $v0, 11\n");             // Syscall code for print character
            fprintf(out, "    syscall\n");
        }
        else if (!strcmp(op, "fwrite")) {
            // float print
            fprintf(out, "    l.s $f12, var_%s\n", c->arg1);
            fprintf(out, "    li $v0, 2\n");
            fprintf(out, "    syscall\n");
        }

        else if (!strcmp(op, "load")) {
            if (c->arg2) {
                // Array access: result = load arr[index]
                // 1. Load base address of array into $s7
                loadArg(c->arg1, "$s7", out);           // Load base address into $s7

                // 2. Load index into $s0
                loadArg(c->arg2, "$s0", out);

                // 3. Multiply index by 4: sll $t0, $s0, 2
                fprintf(out, "    sll $t0, $s0, 2\n");

                // 4. Add base address to get effective address: add $t0, $s7, $t0
                fprintf(out, "    add $t0, $s7, $t0\n");

                // 5. Load word from address: lw $t9, 0($t0)
                fprintf(out, "    lw $t9, 0($t0)\n");

                // 6. Store into result: z = $t9
                storeResult(c->result, "$t9", out);
            }
            else {
                // Simple load: result = load var
                // ***Check if 'c->arg1' is a float var or float temp***
                int isFloat = 0;

                // 1) If it's a temp named "f_#", it's float
                if (isTempName(c->arg1) == 2) {
                    isFloat = 1;
                } else {
                    // 2) If it's a user variable, check the symbol table
                    SymbolType st = lookupTypeInSymbolTable(currentScope, c->arg1);
                    if (st == TYPE_FLOAT) {
                        isFloat = 1;
                    }
                }

                if (isFloat) {
                    // float load
                    loadFloatArg(c->arg1, "$f0", out);
                    // store into c->result
                    storeFloatResult(c->result, "$f0", out);
                } else {
                    // integer load
                    loadArg(c->arg1, "$s0", out);
                    storeResult(c->result, "$s0", out);
                }
            }
        }
        
       else if (!strcmp(op, "store")) {
            // store arg1 into result[arg2]
            // That means: array[result], store arg1 at index arg2

            // 1) Load base address of array into $s7
            loadArg(c->result, "$s7", out); // 'result' is the array name

            // 2) Load index into $s0
            loadArg(c->arg2, "$s0", out);

            // 3) Multiply index by 4: sll $t0, $s0, 2
            fprintf(out, "    sll $t0, $s0, 2\n");

            // 4) Add base address to get effective address: add $t0, $s7, $t0
            fprintf(out, "    add $t0, $s7, $t0\n");

            // 5) Check if arg1 is float or int
            int isFloat = 0;
            if (isTempName(c->arg1) == 2) {
                isFloat = 1;
            } else {
                // If a user-defined variable, check symbol table
                SymbolType st = lookupTypeInSymbolTable(currentScope, c->arg1);
                if (st == TYPE_FLOAT) {
                    isFloat = 1;
                }
            }

            // 6) Based on isFloat, do loadFloatArg + s.s, or loadArg + sw
            if (isFloat) {
                // 6a) Load the float value into $f0
                loadFloatArg(c->arg1, "$f0", out);

                // 6b) Store word into address: s.s $f0, 0($t0)
                fprintf(out, "    s.s %s, 0($t0)\n", "$f0");
            } else {
                // integer store
                // 6a) loadArg => put the integer value into $s1
                loadArg(c->arg1, "$s1", out);

                // 6b) store word: sw $s1, 0($t0)
                fprintf(out, "    sw %s, 0($t0)\n", "$s1");
            }
        }

        else if (!strcmp(op, "ifFalse")) {
            // ifFalse cond goto label
            // chekc if the condition is a float
            if (isEitherFloatTemp(c->arg1, c->arg2)) {
                // 1) Load float arguments into registers, e.g. $f0 and $f1
                loadFloatArg(c->arg1, "$f0", out);
                loadFloatArg(c->arg2, "$f1", out);

                // 2) If it's one of the arithmetic ops (f+, f-, f*, f/), do add.s etc.
                if (!strcmp(op, "ifFalse")) {
                    // Handle equality
                    // Compare $s0 and $s1, set $t9 to 1 if equal, else 0
                    // Generate unique labels
                    char label_true[64];
                    char label_end[64];
                    sprintf(label_true, "L_eq_true_%d", labelCounter++);
                    sprintf(label_end, "L_eq_end_%d", labelCounter++);

                    fprintf(out, "    beq %s, %s, %s\n", "$f0", "$f1", label_true);
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    j %s\n", c->result);
                    fprintf(out, "%s:\n", label_end);
                }
            } else {
            loadArg(c->arg1, "$s0", out);             // Load condition into $s0
            fprintf(out, "    beq $s0, $zero, %s\n", c->result);
            } 
        }
        else if (!strcmp(op, "if")) {
            //check if the condition is a float
            if (isEitherFloatTemp(c->arg1, c->arg2)) {
                // 1) Load float arguments into registers, e.g. $f0 and $f1
                loadFloatArg(c->arg1, "$f0", out);
                loadFloatArg(c->arg2, "$f1", out);

                // 2) If it's one of the arithmetic ops (f+, f-, f*, f/), do add.s etc.
                if (!strcmp(op, "if")) {
                    // Handle equality
                    // Compare $s0 and $s1, set $t9 to 1 if equal, else 0
                    // Generate unique labels
                    char label_true[64];
                    char label_end[64];
                    sprintf(label_true, "L_eq_true_%d", labelCounter++);
                    sprintf(label_end, "L_eq_end_%d", labelCounter++);

                    fprintf(out, "    beq %s, %s, %s\n", "$f0", "$f1", label_true);
                    fprintf(out, "    j %s\n", label_end);
                    fprintf(out, "%s:\n", label_true);
                    fprintf(out, "    j %s\n", c->result);
                    fprintf(out, "%s:\n", label_end);
                }
            } else {
                // if cond goto label
                loadArg(c->arg1, "$s0", out);             // Load condition into $s0
                fprintf(out, "    bne $s0, $zero, %s\n", c->result);
            }
            
        }
        else if (!strcmp(op, "goto")) {
            // Unconditional jump
            fprintf(out, "    j %s\n", c->result);
        }
        else if (!strcmp(op, "label")) {
            // Label definition
            fprintf(out, "%s:\n", c->result);
        }
        else if (!strcmp(op, "call")) {
            // Function call
            // Modify 'jal' to reference 'func_<functionName>'
            fprintf(out, "    jal func_%s\n", c->arg1);
            if (c->result) {
                // Move return value from $v0 to result
                const char* r = getRegIfAny(c->result);
                if (r) {
                    fprintf(out, "    move %s, $v0\n", r);
                } else if (isSpilled(c->result)) {
                    storeResult(c->result, "$v0", out);
                } else {
                    // Should not happen
                    fprintf(stderr, "[ERROR] Unknown binding for %s\n", c->result);
                }
            }
            paramCount = 0;
        }
        else if (!strcmp(op, "return")) {
            // Return statement
            //check if the return value is a float
            if (isTempName(c->arg1) == 2) {
                // Load float return value into $f0
                loadFloatArg(c->arg1, "$f0", out);
                // Instead of emitting 'jr $ra', jump to the function epilogue
                if (strlen(currentFunctionEndLabel) > 0) {
                    fprintf(out, "    j %s\n", currentFunctionEndLabel);
                } else {
                    // If no current function is set, emit 'jr $ra' as a fallback
                    fprintf(out, "    jr $ra\n");
                }
            } else {
                // Load return value into $v0
                loadArg(c->arg1, "$v0", out);
                // Instead of emitting 'jr $ra', jump to the function epilogue
                if (strlen(currentFunctionEndLabel) > 0) {
                    fprintf(out, "    j %s\n", currentFunctionEndLabel);
                } else {
                    // If no current function is set, emit 'jr $ra' as a fallback
                    fprintf(out, "    jr $ra\n");
                }
            }
            
        }
        else if (!strcmp(op, "param")) {
            // Handle function parameters
            // Assuming parameters are passed via $a0..$a3
            //check if the parameter is a float
            if (isTempName(c->arg1) == 2) {
                // Load float parameter into $f0
                loadFloatArg(c->arg1, "$f0", out);
                fprintf(out, "    mov.s $f%d, $f0\n", paramCount);
            } else if (paramCount < 4) {
                loadArg(c->arg1, "$s0", out);         // Load parameter into $s0
                fprintf(out, "    move $a%d, $s0\n", paramCount);
            } else {
                // Handle additional parameters via stack
                // Push them onto the stack
                //check if the parameter is a float
                if (isTempName(c->arg1) == 2) {
                    // Load float parameter into $f0
                    loadFloatArg(c->arg1, "$f0", out);
                    fprintf(out, "    swc1 $f0, 0($sp)\n");
                    fprintf(out, "    addi $sp, $sp, -4\n");
                } else {
                    loadArg(c->arg1, "$s0", out);         // Load parameter into $s0
                    fprintf(out, "    sw $s0, 0($sp)\n");
                    fprintf(out, "    addi $sp, $sp, -4\n");
                }

            }
            paramCount++;
        }
        else {
            // Handle other operators or unhandled TAC
            fprintf(out, "    # Unhandled TAC: %s %s %s %s\n",
                    op,
                    c->arg1 ? c->arg1 : "",
                    c->arg2 ? c->arg2 : "",
                    c->result ? c->result : "");
        }
    }

    // 5) Exit the program
    // After main code, ensure that the program exits properly
    fprintf(out, "    li $v0, 10\n");
    fprintf(out, "    syscall\n");

    fclose(out);
}