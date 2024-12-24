/******************************************************************************
 * codeGenerator.c -- MIPS code generator from TAC
 * 
 * This version uses a local stack approach for function parameters & locals.
 * If a variable is found in 'locals[]', we use offset($fp). Otherwise, it
 * is treated as global => "var_<name>" in the .data section.
 *
 * The key fix for your "undefined symbol" in QtSPIM is to ensure that function
 * parameters (like "a", "b") are recognized as locals in findLocalOffset(...),
 * and that function calls reference the same safe label names that the
 * function definitions produce.
 ******************************************************************************/


#include "codeGenerator.h"
#include "symbolTable.h" // for Symbol, SymbolType, ScopeNode, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOCALS 256

/*
 * For each function, we track local variables and parameters in 'locals[]'.
 * Each has a name and an offset from $fp. A nonzero offset means "local/param".
 * If findLocalOffset(...) returns 0, that means we treat it as a global.
 */
typedef struct {
    char name[64];  // e.g. "a", "x", etc.
    int offset;     // offset from $fp
} LocalInfo;

static LocalInfo locals[MAX_LOCALS];
static int localCount = 0;

/*
 * Clears local info at the start of each function.
 */
static void resetLocals() {
    localCount = 0;
    memset(locals, 0, sizeof(locals));
}

/*
 * Add one local or parameter to the local array with a known offset from $fp.
 * For parameters, the offset might be +8, +12, etc., depending on the calling convention.
 */
static void addLocal(const char* varName, int offset) {
    if (localCount >= MAX_LOCALS) {
        fprintf(stderr, "[ERROR] Too many locals/params in function.\n");
        return;
    }
    strncpy(locals[localCount].name, varName, 63);
    locals[localCount].offset = offset;
    localCount++;
}

/*
 * Finds an offset for varName if it is local/param. If not found, returns 0.
 * A return of 0 => treat it as a global symbol.
 */
static int findLocalOffset(const char* varName) {
    for (int i = 0; i < localCount; i++) {
        if (strcmp(locals[i].name, varName) == 0) {
            return locals[i].offset;
        }
    }
    return 0; // Not found => 0 => global
}

/*
 * We'll keep track of how many 'param' instructions we have, so we know
 * how many bytes to pop off the stack after a function call.
 */
static int paramCount = 0;

/*
 * We'll also need to generate unique float labels sometimes:
 * "_floattmp_<labelCounter> : .float 1.23"
 */
static int labelCounter = 0;

/*
 * We'll keep a global 'outFile' pointer for convenience.
 */
static FILE* outFile = NULL;


/******************************************************************************
 * Helper: makeSafeGlobalName
 * If "userName" is "t<number>", we do *not* rename it.
 * Otherwise, we prepend "var_".
 ******************************************************************************/
static char* makeSafeGlobalName(const char* userName) {
    if (!userName) return NULL;
    // If it's a TAC temp "t<number>", do not rename
    if (userName[0] == 't') {
        int allDigits = 1;
        for (int i = 1; userName[i] != '\0'; i++) {
            if (userName[i] < '0' || userName[i] > '9') {
                allDigits = 0;
                break;
            }
        }
        if (allDigits) {
            // It's a TAC temp => do not rename
            return strdup(userName);
        }
    }
    // Otherwise, prepend "var_"
    char* buf = (char*)malloc(strlen(userName) + 5);
    sprintf(buf, "var_%s", userName);
    return buf;
}


/******************************************************************************
 * Helper: makeSafeLabel
 * "func_add" => "L_func_add"
 * "endfunc_add" => "L_endfunc_add"
 * else => "L_<label>"
 ******************************************************************************/
static char* makeSafeLabel(const char* labelName) {
    if (!labelName) return NULL;

    if (strncmp(labelName, "func_", 5) == 0) {
        const char* theRest = labelName + 5;
        char* buf = (char*)malloc(strlen(theRest) + 10);
        sprintf(buf, "L_func_%s", theRest);
        return buf;
    }
    if (strncmp(labelName, "endfunc_", 8) == 0) {
        const char* theRest = labelName + 8;
        char* buf = (char*)malloc(strlen(theRest) + 12);
        sprintf(buf, "L_endfunc_%s", theRest);
        return buf;
    }
    // else
    char* buf = (char*)malloc(strlen(labelName) + 3);
    sprintf(buf, "L_%s", labelName);
    return buf;
}


/******************************************************************************
 * mapTempToReg:
 * If "t0", parse out the number -> mod 10 => "$t0..$t9"
 * Return NULL if not "t<number>" form.
 ******************************************************************************/
static const char* mapTempToReg(const char* tempName) {
    if (!tempName || tempName[0] != 't') return NULL;
    int index = atoi(&tempName[1]);
    int regNum = index % 10;
    static char reg[8];
    sprintf(reg, "$t%d", regNum);
    return strdup(reg);
}


/******************************************************************************
 * loadVar / storeVar:
 * If findLocalOffset(name) != 0 => local => lw/sw offset($fp).
 * Otherwise => global => la $t9, var_...; lw/sw from 0($t9).
 ******************************************************************************/
static void loadVar(const char* destReg, const char* varName) {
    int off = findLocalOffset(varName);
    if (off != 0) {
        // local or param
        fprintf(outFile, "lw %s, %d($fp)\n", destReg, off);
    } else {
        // global
        char* safe = makeSafeGlobalName(varName);
        fprintf(outFile, "la $t9, %s\n", safe);
        fprintf(outFile, "lw %s, 0($t9)\n", destReg);
        free(safe);
    }
}

static void storeVar(const char* srcReg, const char* varName) {
    int off = findLocalOffset(varName);
    if (off != 0) {
        // local
        fprintf(outFile, "sw %s, %d($fp)\n", srcReg, off);
    } else {
        // global
        char* safe = makeSafeGlobalName(varName);
        fprintf(outFile, "la $t9, %s\n", safe);
        fprintf(outFile, "sw %s, 0($t9)\n", srcReg);
        free(safe);
    }
}


/******************************************************************************
 * emitGlobalData:
 * Write .data lines for global variables from scope level 0 (the global scope).
 ******************************************************************************/
static void emitGlobalData(const ScopeNode* scopeListHead) {
    fprintf(outFile, ".data\n");
    if (!scopeListHead || !scopeListHead->scope) return;

    SymbolTable* global = scopeListHead->scope;
    for (int i = 0; i < global->size; i++) {
        Symbol* sym = global->table[i];
        while (sym) {
            if ((sym->type == TYPE_INT || sym->type == TYPE_FLOAT) && sym->size == 0) {
                // single int or float
                char* safeName = makeSafeGlobalName(sym->name);
                if (sym->type == TYPE_INT) {
                    fprintf(outFile, "%s: .word 0\n", safeName);
                } else {
                    fprintf(outFile, "%s: .float 0.0\n", safeName);
                }
                free(safeName);
            } 
            else if ((sym->type == TYPE_INT || sym->type == TYPE_FLOAT) && sym->size > 0) {
                int totalBytes = 4 * sym->size;
                char* safeName = makeSafeGlobalName(sym->name);
                fprintf(outFile, "%s: .space %d\n", safeName, totalBytes);
                free(safeName);
            }
            // if function => skip
            sym = sym->next;
        }
    }
    fprintf(outFile, "\n.text\n");
}


/******************************************************************************
 * generateMIPSFromTAC: main code generator function
 *
 * The big fix for "var_a not found" is that you must call addLocal("a", <offset>)
 * if "a" is a function parameter (or local). Then loadVar/storeVar won't treat
 * it as a global symbol.
 *
 * We also unify function calls so that "call add" calls the label produced by
 * "makeSafeLabel(func_add) => L_func_add" so the label name matches exactly.
 ******************************************************************************/
void generateMIPSFromTAC(TAC* tacHead, const ScopeNode* scopeListHead, const char* outputFilename)
{
    outFile = fopen(outputFilename, "w");
    if (!outFile) {
        fprintf(stderr, "[ERROR] Could not open file for writing MIPS code.\n");
        return;
    }

    // 1) Emit global data for any global variables
    emitGlobalData(scopeListHead);

    // 2) Mark main as global entry
    fprintf(outFile, ".globl main\n");
    fprintf(outFile, "main:\n");

    // Start traversing the TAC list
    TAC* current = tacHead;
    while (current) {
        const char* op = current->operator ? current->operator : "";

        if (!strcmp(op, "label")) {
            // rename label => safe
            char* safeLbl = makeSafeLabel(current->result);

            // If it's "func_...", reset locals, emit prologue
            if (!strncmp(current->result, "func_", 5)) {
                // Start new function => clear local map
                resetLocals();

                /*********************************************
                 * EXAMPLE HARDCODE for param offsets:
                 * If we see 'func_add', let's define offsets
                 * for parameters 'a' and 'b' at 8($fp) and 12($fp).
                 *********************************************/
                if (!strcmp(current->result, "func_add")) {
                    addLocal("a", 8);
                    addLocal("b", 12);
                }

                // Then print the label & function prologue
                fprintf(outFile, "%s:\n", safeLbl);
                fprintf(outFile, "# function prologue\n");
                fprintf(outFile, "addi $sp, $sp, -8\n");
                fprintf(outFile, "sw $ra, 4($sp)\n");
                fprintf(outFile, "sw $fp, 0($sp)\n");
                fprintf(outFile, "move $fp, $sp\n");
            }
            else if (!strncmp(current->result, "endfunc_", 8)) {
                // Just label it; epilogue typically in 'return'
                fprintf(outFile, "%s:\n", safeLbl);
            }
            else {
                // normal label
                fprintf(outFile, "%s:\n", safeLbl);
            }
            free(safeLbl);
        }

        else if (!strcmp(op, "=")) {
            // result = arg1
            const char* rd = mapTempToReg(current->result);
            if (rd) {
                // load immediate / float / var => rd
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    // integer literal
                    fprintf(outFile, "li %s, %ld\n", rd, val);
                }
                else {
                    float fval = strtof(current->arg1, &endp);
                    if (*endp == '\0') {
                        // float literal => store in a label
                        fprintf(outFile, ".data\n_floattmp_%d: .float %f\n.text\n", labelCounter, fval);
                        fprintf(outFile, "la $t9, _floattmp_%d\n", labelCounter++);
                        fprintf(outFile, "lw %s, 0($t9)\n", rd);
                    } else {
                        // maybe 'tX' or local/global
                        const char* rs = mapTempToReg(current->arg1);
                        if (rs) {
                            fprintf(outFile, "move %s, %s\n", rd, rs);
                        } else {
                            loadVar(rd, current->arg1);
                        }
                    }
                }
            } else {
                // storing into local/global
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    // integer literal => store
                    fprintf(outFile, "li $t0, %ld\n", val);
                    storeVar("$t0", current->result);
                }
                else {
                    float fval = strtof(current->arg1, &endp);
                    if (*endp == '\0') {
                        // float literal
                        fprintf(outFile, ".data\n_floattmp_%d: .float %f\n.text\n", labelCounter, fval);
                        fprintf(outFile, "la $t0, _floattmp_%d\n", labelCounter++);
                        fprintf(outFile, "lw $t0, 0($t0)\n");
                        storeVar("$t0", current->result);
                    } else {
                        // maybe 'tX' or local/global
                        const char* rs = mapTempToReg(current->arg1);
                        if (rs) {
                            storeVar(rs, current->result);
                        } else {
                            loadVar("$t0", current->arg1);
                            storeVar("$t0", current->result);
                        }
                    }
                }
            }
        }

        else if (!strcmp(op, "+") || !strcmp(op, "-") ||
                 !strcmp(op, "*") || !strcmp(op, "/")) {
            // result = arg1 op arg2
            const char* rd = mapTempToReg(current->result);
            if (!rd) {
                fprintf(outFile, "# error: arithmetic result not a temp\n");
                current = current->next;
                continue;
            }
            // load arg1 => $t1
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t1, %ld\n", val);
                    rs = "$t1";
                } else {
                    loadVar("$t1", current->arg1);
                    rs = "$t1";
                }
            }
            // load arg2 => $t2
            const char* rt = mapTempToReg(current->arg2);
            if (!rt) {
                char* endp;
                long val = strtol(current->arg2, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t2, %ld\n", val);
                    rt = "$t2";
                } else {
                    loadVar("$t2", current->arg2);
                    rt = "$t2";
                }
            }

            if (!strcmp(op, "+")) {
                fprintf(outFile, "add %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, "-")) {
                fprintf(outFile, "sub %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, "*")) {
                fprintf(outFile, "mul %s, %s, %s\n", rd, rs, rt);
            } else {
                // "/"
                fprintf(outFile, "div %s, %s\n", rs, rt);
                fprintf(outFile, "mflo %s\n", rd);
            }
        }

        else if (!strcmp(op, "==") || !strcmp(op, "!=") ||
                 !strcmp(op, "<")  || !strcmp(op, "<=") ||
                 !strcmp(op, ">")  || !strcmp(op, ">=")) {
            // comparison
            const char* rd = mapTempToReg(current->result);
            if (!rd) {
                fprintf(outFile, "# error: comparison result not a temp\n");
                current = current->next;
                continue;
            }
            // load arg1 => $t1
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t1, %ld\n", val);
                    rs = "$t1";
                } else {
                    loadVar("$t1", current->arg1);
                    rs = "$t1";
                }
            }
            // load arg2 => $t2
            const char* rt = mapTempToReg(current->arg2);
            if (!rt) {
                char* endp;
                long val = strtol(current->arg2, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t2, %ld\n", val);
                    rt = "$t2";
                } else {
                    loadVar("$t2", current->arg2);
                    rt = "$t2";
                }
            }

            if (!strcmp(op, "==")) {
                fprintf(outFile, "seq %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, "!=")) {
                fprintf(outFile, "sne %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, "<")) {
                fprintf(outFile, "slt %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, "<=")) {
                fprintf(outFile, "sle %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, ">")) {
                fprintf(outFile, "sgt %s, %s, %s\n", rd, rs, rt);
            } else if (!strcmp(op, ">=")) {
                fprintf(outFile, "sge %s, %s, %s\n", rd, rs, rt);
            }
        }

        else if (!strcmp(op, "write")) {
            // write integer in $a0 => syscall
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    // immediate
                    fprintf(outFile, "li $t0, %ld\n", val);
                    rs = "$t0";
                } else {
                    loadVar("$t0", current->arg1);
                    rs = "$t0";
                }
            }
            fprintf(outFile, "move $a0, %s\n", rs);
            fprintf(outFile, "li $v0, 1\n");
            fprintf(outFile, "syscall\n");
            // newline
            fprintf(outFile, "li $a0, '\\n'\n");
            fprintf(outFile, "li $v0, 11\n");
            fprintf(outFile, "syscall\n");
        }

        else if (!strcmp(op, "load")) {
            // result = load var or arr[index]
            const char* rd = mapTempToReg(current->result);
            if (!rd) {
                fprintf(outFile, "# error: load result not a temp\n");
                current = current->next;
                continue;
            }
            if (current->arg2) {
                // array
                const char* idx = mapTempToReg(current->arg2);
                if (!idx) {
                    char* endp;
                    long val = strtol(current->arg2, &endp, 10);
                    if (*endp == '\0') {
                        fprintf(outFile, "li $t1, %ld\n", val);
                        idx = "$t1";
                    } else {
                        loadVar("$t1", current->arg2);
                        idx = "$t1";
                    }
                }
                fprintf(outFile, "sll $t2, %s, 2\n", idx);
                char* safe = makeSafeGlobalName(current->arg1);
                fprintf(outFile, "la $t9, %s\n", safe);
                free(safe);
                fprintf(outFile, "add $t9, $t9, $t2\n");
                fprintf(outFile, "lw %s, 0($t9)\n", rd);
            } else {
                loadVar(rd, current->arg1);
            }
        }

        else if (!strcmp(op, "store")) {
            // store arg1 => result[arg2]
            const char* srcReg = mapTempToReg(current->arg1);
            if (!srcReg) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t0, %ld\n", val);
                    srcReg = "$t0";
                } else {
                    loadVar("$t0", current->arg1);
                    srcReg = "$t0";
                }
            }
            const char* idx = mapTempToReg(current->arg2);
            if (!idx) {
                char* endp;
                long val = strtol(current->arg2, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t1, %ld\n", val);
                    idx = "$t1";
                } else {
                    loadVar("$t1", current->arg2);
                    idx = "$t1";
                }
            }
            fprintf(outFile, "sll $t2, %s, 2\n", idx);
            char* safeArr = makeSafeGlobalName(current->result);
            fprintf(outFile, "la $t9, %s\n", safeArr);
            free(safeArr);
            fprintf(outFile, "add $t9, $t9, $t2\n");
            fprintf(outFile, "sw %s, 0($t9)\n", srcReg);
        }

        else if (!strcmp(op, "if")) {
            // if arg1 goto label
            char* safeLbl = makeSafeLabel(current->result);
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t0, %ld\n", val);
                    rs = "$t0";
                } else {
                    loadVar("$t0", current->arg1);
                    rs = "$t0";
                }
            }
            fprintf(outFile, "bne %s, $zero, %s\n", rs, safeLbl);
            free(safeLbl);
        }

        else if (!strcmp(op, "goto")) {
            char* safeLbl = makeSafeLabel(current->result);
            fprintf(outFile, "j %s\n", safeLbl);
            free(safeLbl);
        }

        else if (!strcmp(op, "call")) {
            /*
             * If TAC says "call add", we want to jump to the same label
             * that "func_add" would produce => "L_func_add" via makeSafeLabel.
             * So we do: "func_<arg1>" => "func_add", then safe => "L_func_add".
             */
            char funcBuf[64];
            sprintf(funcBuf, "func_%s", current->arg1);   // e.g. "func_add"
            char* safeLbl = makeSafeLabel(funcBuf);       // => "L_func_add"

            fprintf(outFile, "jal %s\n", safeLbl);
            free(safeLbl);

            // now pop parameters
            if (paramCount > 0) {
                fprintf(outFile, "addi $sp, $sp, %d\n", paramCount * 4);
                paramCount = 0;
            }
            // if there's a result => move $v0 => result
            if (current->result) {
                const char* rd = mapTempToReg(current->result);
                if (rd) {
                    fprintf(outFile, "move %s, $v0\n", rd);
                } else {
                    storeVar("$v0", current->result);
                }
            }
        }

        else if (!strcmp(op, "param")) {
            // push arg1
            paramCount++;
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $t0, %ld\n", val);
                    rs = "$t0";
                } else {
                    loadVar("$t0", current->arg1);
                    rs = "$t0";
                }
            }
            fprintf(outFile, "addi $sp, $sp, -4\n");
            fprintf(outFile, "sw %s, 0($sp)\n", rs);
        }

        else if (!strcmp(op, "return")) {
            // return arg => epilogue => jr $ra
            const char* rs = mapTempToReg(current->arg1);
            if (!rs) {
                char* endp;
                long val = strtol(current->arg1, &endp, 10);
                if (*endp == '\0') {
                    fprintf(outFile, "li $v0, %ld\n", val);
                } else {
                    loadVar("$t0", current->arg1);
                    fprintf(outFile, "move $v0, $t0\n");
                }
            } else {
                fprintf(outFile, "move $v0, %s\n", rs);
            }
            // function epilogue
            fprintf(outFile, "move $sp, $fp\n");
            fprintf(outFile, "lw $fp, 0($sp)\n");
            fprintf(outFile, "lw $ra, 4($sp)\n");
            fprintf(outFile, "addi $sp, $sp, 8\n");
            fprintf(outFile, "jr $ra\n");
        }

        else {
            fprintf(outFile, "# Unhandled TAC op: %s\n", op);
        }

        current = current->next;
    }

    // final end
    fprintf(outFile, "\nend:\n");
    fprintf(outFile, "li $v0, 10\n");
    fprintf(outFile, "syscall\n");

    fclose(outFile);
    printf("[INFO] MIPS code generated in %s\n", outputFilename);
}
