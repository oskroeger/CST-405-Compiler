#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"  // for TAC definition

/**************************************
 * Helpers
 **************************************/

// Map "t<number>" => "$t0..$t9" by (number % 10).
// If it's not "t<number>", return NULL => treat as global or immediate.
static const char* mapTempToReg(const char* name) {
    if (!name) return NULL;
    if (name[0] != 't') return NULL;  // e.g. "x" => not a temp
    int index = atoi(name + 1);
    int regNum = index % 10;
    static char reg[8];
    sprintf(reg, "$t%d", regNum);
    return strdup(reg);
}

// Convert a user variable name "x" => "var_x".
static char* makeGlobalName(const char* var) {
    if (!var) return NULL;
    // If it's a TAC temp "t<number>", do NOT rename:
    if (var[0] == 't') {
        int allDigits = 1;
        for (int i = 1; var[i] != '\0'; i++) {
            if (var[i] < '0' || var[i] > '9') {
                allDigits = 0;
                break;
            }
        }
        if (allDigits) {
            // It's a temp => do not rename
            return strdup(var);
        }
    }
    // Otherwise, prepend "var_"
    char* buf = (char*)malloc(strlen(var) + 5);
    sprintf(buf, "var_%s", var);
    return buf;
}

/*
 * We'll gather all global variables from TAC instructions where:
 *   1) operator is "=" and result is not a temp
 *   2) operator is "write" and arg1 is not a temp
 * Then we create ".word 0" entries for each one in .data.
 */
static void collectGlobalVars(TAC* tac, FILE* out) {
    fprintf(out, ".data\n");
    // Keep track of which variable names we've seen so we don't duplicate.
    // In a real system, you'd use a dynamic structure; for simplicity, we do a small array.
    char seen[100][64];
    int seenCount = 0;

    for (TAC* c = tac; c; c = c->next) {
        const char* op = (c->operator ? c->operator : "");
        // Case 1: "result = arg1"
        if (!strcmp(op, "=") && c->result) {
            const char* maybeReg = mapTempToReg(c->result);
            if (!maybeReg) {
                // It's a global var (like "x") => track it
                int already = 0;
                for (int i = 0; i < seenCount; i++) {
                    if (!strcmp(seen[i], c->result)) {
                        already = 1;
                        break;
                    }
                }
                if (!already) {
                    strcpy(seen[seenCount++], c->result);
                }
            }
        }
        // Case 2: "write arg1"
        if (!strcmp(op, "write") && c->arg1) {
            const char* maybeReg = mapTempToReg(c->arg1);
            if (!maybeReg) {
                int already = 0;
                for (int i = 0; i < seenCount; i++) {
                    if (!strcmp(seen[i], c->arg1)) {
                        already = 1;
                        break;
                    }
                }
                if (!already) {
                    strcpy(seen[seenCount++], c->arg1);
                }
            }
        }
    }

    // Now emit each var as "var_x: .word 0"
    for (int i = 0; i < seenCount; i++) {
        char* safeName = makeGlobalName(seen[i]);
        fprintf(out, "%s: .word 0\n", safeName);
        free(safeName);
    }
    fprintf(out, "\n.text\n");
}

/*
 * Main code generator:
 *   - We produce .data for global variables
 *   - Then we produce "main:" (assuming a single program)
 *   - For each TAC instruction, we handle only =, +, -, *, /, and write
 *   - We do a final "li $v0, 10 ; syscall" at the end
 */
void generateMIPSFromTAC(TAC* tacHead, const char* outputFilename) {
    FILE* out = fopen(outputFilename, "w");
    if (!out) {
        fprintf(stderr, "[ERROR] Could not open %s for writing MIPS code\n", outputFilename);
        return;
    }

    // 1) Emit .data for all global variables we find
    collectGlobalVars(tacHead, out);

    // 2) Start .text and define main
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");

    // 3) Process each TAC
    for (TAC* c = tacHead; c; c = c->next) {
        const char* op = (c->operator ? c->operator : "");

        // (a) handle "result = arg1"
        if (!strcmp(op, "=")) {
            const char* rd = mapTempToReg(c->result);   // destination register, if any
            // parse arg1 => check if it's an integer literal
            char* endp;
            long val = strtol(c->arg1, &endp, 10);
            if (*endp == '\0') {
                // arg1 is an integer literal
                if (rd) {
                    // e.g. "li $t0, 4"
                    fprintf(out, "li %s, %ld\n", rd, val);
                } else {
                    // store to a global var => "li $t0, val; sw $t0, var_..."
                    fprintf(out, "li $t0, %ld\n", val);
                    char* g = makeGlobalName(c->result);
                    fprintf(out, "la $t9, %s\n", g);
                    fprintf(out, "sw $t0, 0($t9)\n");
                    free(g);
                }
            } else {
                // not an integer => could be a temp or another global
                const char* rs = mapTempToReg(c->arg1);
                if (rs) {
                    // arg1 is a register temp
                    if (rd) {
                        // both sides are temps => "move rd, rs"
                        fprintf(out, "move %s, %s\n", rd, rs);
                    } else {
                        // result is global => "sw rs, var_..."
                        char* gRes = makeGlobalName(c->result);
                        fprintf(out, "la $t9, %s\n", gRes);
                        fprintf(out, "sw %s, 0($t9)\n", rs);
                        free(gRes);
                    }
                } else {
                    // arg1 is global => load it => store
                    char* gArg = makeGlobalName(c->arg1);
                    if (rd) {
                        // load into register
                        fprintf(out, "la $t9, %s\n", gArg);
                        fprintf(out, "lw %s, 0($t9)\n", rd);
                    } else {
                        // global=global => use $t0 as scratch
                        fprintf(out, "la $t9, %s\n", gArg);
                        fprintf(out, "lw $t0, 0($t9)\n");
                        char* gRes = makeGlobalName(c->result);
                        fprintf(out, "la $t9, %s\n", gRes);
                        fprintf(out, "sw $t0, 0($t9)\n");
                        free(gRes);
                    }
                    free(gArg);
                }
            }
        }

        // (b) handle "result = arg1 op arg2" for +, -, *, /
        else if (!strcmp(op, "+") || !strcmp(op, "-") ||
                 !strcmp(op, "*") || !strcmp(op, "/")) {
            const char* rd = mapTempToReg(c->result);
            if (!rd) {
                fprintf(out, "# error: result %s is not a temp for arithmetic.\n", c->result);
                continue;
            }
            // load arg1 => $t1
            char* endp;
            long val = strtol(c->arg1, &endp, 10);
            if (*endp == '\0') {
                // integer
                fprintf(out, "li $t1, %ld\n", val);
            } else {
                // either temp or global
                const char* rs = mapTempToReg(c->arg1);
                if (rs) {
                    fprintf(out, "move $t1, %s\n", rs);
                } else {
                    char* gArg1 = makeGlobalName(c->arg1);
                    fprintf(out, "la $t9, %s\n", gArg1);
                    fprintf(out, "lw $t1, 0($t9)\n");
                    free(gArg1);
                }
            }
            // load arg2 => $t2
            char* endp2;
            long val2 = strtol(c->arg2, &endp2, 10);
            if (*endp2 == '\0') {
                // integer
                fprintf(out, "li $t2, %ld\n", val2);
            } else {
                // temp or global
                const char* rt = mapTempToReg(c->arg2);
                if (rt) {
                    fprintf(out, "move $t2, %s\n", rt);
                } else {
                    char* gArg2 = makeGlobalName(c->arg2);
                    fprintf(out, "la $t9, %s\n", gArg2);
                    fprintf(out, "lw $t2, 0($t9)\n");
                    free(gArg2);
                }
            }

            // do the operation => store in rd
            if (!strcmp(op, "+")) {
                fprintf(out, "add %s, $t1, $t2\n", rd);
            } else if (!strcmp(op, "-")) {
                fprintf(out, "sub %s, $t1, $t2\n", rd);
            } else if (!strcmp(op, "*")) {
                fprintf(out, "mul %s, $t1, $t2\n", rd);
            } else {
                // "/"
                fprintf(out, "div $t1, $t2\n");
                fprintf(out, "mflo %s\n", rd);
            }
        }

        // (c) handle "write arg1"
        else if (!strcmp(op, "write")) {
            // if arg1 is a temp => move $a0, that
            // else if int => li => move
            // else if global => lw => move
            const char* rs = mapTempToReg(c->arg1);
            if (rs) {
                fprintf(out, "move $a0, %s\n", rs);
            } else {
                // maybe an integer or a global
                char* endp;
                long val = strtol(c->arg1, &endp, 10);
                if (*endp == '\0') {
                    // immediate
                    fprintf(out, "li $t0, %ld\n", val);
                    fprintf(out, "move $a0, $t0\n");
                } else {
                    // global
                    char* gArg = makeGlobalName(c->arg1);
                    fprintf(out, "la $t9, %s\n", gArg);
                    fprintf(out, "lw $t0, 0($t9)\n");
                    fprintf(out, "move $a0, $t0\n");
                    free(gArg);
                }
            }
            // print int
            fprintf(out, "li $v0, 1\n");
            fprintf(out, "syscall\n");
            // newline
            fprintf(out, "li $a0, '\\n'\n");
            fprintf(out, "li $v0, 11\n");
            fprintf(out, "syscall\n");
        }

        else {
            // We ignore any instructions outside of { =, +, -, *, /, write } for now.
            fprintf(out, "# Unhandled TAC: %s %s %s %s\n",
                op,
                c->arg1 ? c->arg1 : "",
                c->arg2 ? c->arg2 : "",
                c->result? c->result : "");
        }
    }

    // 4) End with exit
    fprintf(out, "\n# exit\n");
    fprintf(out, "li $v0, 10\n");
    fprintf(out, "syscall\n");

    fclose(out);
    printf("[INFO] MIPS code generated in %s\n", outputFilename);
}
