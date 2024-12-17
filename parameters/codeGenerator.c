#include "codeGenerator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
// Structures for handling float literals storage
typedef struct FloatLit {
    char* label;
    float value;
    struct FloatLit* next;
} FloatLit;

static FloatLit* floatLits = NULL;
static int floatLitCount = 0;

static char* addFloatLit(float f) {
    FloatLit* fl = (FloatLit*)malloc(sizeof(FloatLit));
    fl->value = f;
    char buf[32];
    sprintf(buf,"FLT%d", floatLitCount++);
    fl->label = strdup(buf);
    fl->next = floatLits;
    floatLits = fl;
    return fl->label;
}

// Determine if a string is an immediate integer
static int isIntImmediate(const char* s) {
    if (!s) return 0;
    // Negative/positive integers allowed
    for (int i=0; s[i]; i++) {
        if (!(isdigit(s[i]) || s[i]=='-' || s[i]=='+')) return 0;
    }
    return 1;
}

// Determine if a string is a float literal
static int isFloatLiteral(const char* s) {
    return (s && strchr(s,'.')!=NULL);
}

// Is a temp like t0,t1,...
static int isTemp(const char* s) {
    return (s && s[0]=='t' && isdigit(s[1]));
}

// Map temp to registers
// For ints: $t0-$t9
// For floats: $f0-$f9
static char* getIntRegForTemp(const char* temp) {
    int num = atoi(temp+1);
    int regNum = num % 10;
    static char regName[5];
    sprintf(regName, "$t%d", regNum);
    return strdup(regName);
}

static char* getFloatRegForTemp(const char* temp) {
    int num = atoi(temp+1);
    int regNum = num % 10;
    static char regName[5];
    sprintf(regName,"$f%d", regNum);
    return strdup(regName);
}

// Retrieve symbol information
// You must ensure currentScope or global symbol structures are accessible
// We'll assume `lookupSymbol(currentScope, name)` is available as per semantic/h
extern SymbolTable* currentScope;
static Symbol* getSymbolInfo(const char* name) {
    if (!name) return NULL;
    Symbol* s = lookupSymbol(currentScope,(char*)name);
    return s;
}

static SymbolType getSymbolType(const char* name) {
    Symbol* s = getSymbolInfo(name);
    if (!s) return TYPE_UNKNOWN;
    return s->type;
}

// For arrays, get size from symbol table
static int getArraySize(const char* name) {
    Symbol* s = getSymbolInfo(name);
    if (!s) return 10; // default if not found
    return s->size;
}

// Print all variables in .data
// We'll do a scan of TAC to find variables. Instead, rely on symbol table printing or track them ourselves.
static void printFloatLits(FILE* out) {
    FloatLit* fl = floatLits;
    while(fl) {
        fprintf(out,"%s: .float %f\n", fl->label, fl->value);
        fl=fl->next;
    }
}

// We need a pass to find all variables and arrays from TAC and symbol table and print them.
// Let's assume we print all symbols from symbol table:
extern ScopeNode* scopeListHead;
static void printDataSection(FILE* out) {
    fprintf(out, ".data\n");

    // Print all symbols from all scopes
    // We'll do a simple traversal
    ScopeNode* sc = scopeListHead;
    while (sc) {
        SymbolTable* st = sc->scope;
        if (st && st->table) {
            for (int i=0; i<st->size; i++) {
                for (Symbol* sym = st->table[i]; sym; sym=sym->next) {
                    // Print only if TYPE_INT or TYPE_FLOAT and not printed before
                    // We can track if printed using a static list
                    // For simplicity, assume no duplicates

                    // Already printed arrays or vars would appear multiple times if multiple scopes share names
                    // but let's assume distinct names or global unique naming.

                    SymbolType t = sym->type;
                    if (t==TYPE_INT || t==TYPE_FLOAT) {
                        if (sym->size>0) {
                            // array
                            fprintf(out,"%s: ", sym->name);
                            if (t==TYPE_FLOAT) {
                                fprintf(out,".float ");
                                for (int j=0; j<sym->size; j++) {
                                    fprintf(out,"0.0%s",(j<sym->size-1)?", ":"");
                                }
                            } else {
                                fprintf(out,".word ");
                                for (int j=0; j<sym->size; j++) {
                                    fprintf(out,"0%s",(j<sym->size-1)?", ":"");
                                }
                            }
                            fprintf(out,"\n");
                        } else {
                            // single var
                            if (t==TYPE_FLOAT) {
                                fprintf(out,"%s: .float 0.0\n", sym->name);
                            } else {
                                fprintf(out,"%s: .word 0\n", sym->name);
                            }
                        }
                    }
                    if (t==TYPE_FUNCTION) {
                        // no data needed
                    }
                }
            }
        }
        sc=sc->next;
    }

    printFloatLits(out);
}

// Load an operand into a register based on its symbol type or literal
static char* loadOperand(FILE* out, const char* operand) {
    if (!operand) return NULL;

    // Determine type:
    // If immediate int => TYPE_INT
    // If float literal => TYPE_FLOAT
    // Else lookup symbol type
    SymbolType t = TYPE_INT; 
    if (isFloatLiteral(operand)) t=TYPE_FLOAT;
    else if (!isIntImmediate(operand) && !isTemp(operand)) {
        t = getSymbolType(operand);
    }

    if (isIntImmediate(operand) && t==TYPE_INT) {
        fprintf(out,"li $t0, %s\n",operand);
        return "$t0";
    } else if (isFloatLiteral(operand) && t==TYPE_FLOAT) {
        float f=(float)atof(operand);
        char* lbl=addFloatLit(f);
        fprintf(out,"lwc1 $f0, %s\n",lbl);
        return "$f0";
    } else if (isTemp(operand)) {
        if (t==TYPE_FLOAT) {
            return getFloatRegForTemp(operand);
        } else {
            return getIntRegForTemp(operand);
        }
    } else {
        // variable
        if (t==TYPE_FLOAT) {
            fprintf(out,"lwc1 $f0, %s\n", operand);
            return "$f0";
        } else {
            fprintf(out,"lw $t0, %s\n", operand);
            return "$t0";
        }
    }
}

// Store value into var
static void storeValue(FILE* out, const char* varName, const char* srcReg) {
    SymbolType t = getSymbolType(varName);
    if (t==TYPE_FLOAT) {
        fprintf(out,"swc1 %s, %s\n",srcReg,varName);
    } else {
        fprintf(out,"sw %s, %s\n",srcReg,varName);
    }
}

// Load array element: arr[index]
static void loadArrayElement(FILE* out, const char* arrName, const char* index) {
    SymbolType t = getSymbolType(arrName);
    int arrSize = getArraySize(arrName);
    (void)arrSize; // we don't necessarily need it unless for bounds checking

    char* idxReg = NULL;
    if (isIntImmediate(index)) {
        int idx=atoi(index);
        int offset=idx*4;
        if (t==TYPE_FLOAT) {
            fprintf(out,"lwc1 $f0, %s+%d\n",arrName,offset);
        } else {
            fprintf(out,"lw $t0, %s+%d\n",arrName,offset);
        }
    } else {
        char* ir = loadOperand(out,index); 
        // ir should be int register now
        // if it's float register by mistake, we'd need to convert. We'll assume indexes are int typed.

        fprintf(out,"sll $t2, %s, 2\n", ir);
        fprintf(out,"la $t3, %s\n", arrName);
        fprintf(out,"add $t3, $t3, $t2\n");
        if (t==TYPE_FLOAT) {
            fprintf(out,"lwc1 $f0, 0($t3)\n");
        } else {
            fprintf(out,"lw $t0, 0($t3)\n");
        }
    }
}

// Store array element: arr[index] = value
static void storeArrayElement(FILE* out, const char* arrName, const char* index, const char* value) {
    SymbolType t = getSymbolType(arrName);
    char* valReg = loadOperand(out,value);
    // valReg is now in either $t0 or $f0 depending on type
    if (isIntImmediate(index)) {
        int idx = atoi(index);
        int offset = idx*4;
        if (t==TYPE_FLOAT) {
            fprintf(out,"swc1 %s, %s+%d\n", valReg, arrName, offset);
        } else {
            fprintf(out,"sw %s, %s+%d\n", valReg, arrName, offset);
        }
    } else {
        char* ir = loadOperand(out,index); 
        fprintf(out,"sll $t2, %s, 2\n", ir);
        fprintf(out,"la $t3, %s\n", arrName);
        fprintf(out,"add $t3, $t3, $t2\n");
        if (t==TYPE_FLOAT) {
            fprintf(out,"swc1 %s, 0($t3)\n", valReg);
        } else {
            fprintf(out,"sw %s, 0($t3)\n", valReg);
        }
    }
}

// Print a value (write)
static void printValue(FILE* out, const char* operand) {
    SymbolType t = TYPE_INT;
    if (isFloatLiteral(operand)) t=TYPE_FLOAT;
    else if (!isIntImmediate(operand) && !isTemp(operand)) {
        t = getSymbolType(operand);
    }

    char* valReg = loadOperand(out,operand);
    if (t==TYPE_FLOAT) {
        // print float
        fprintf(out,"mov.s $f12, %s\n", valReg);
        fprintf(out,"li $v0, 2\n");
        fprintf(out,"syscall\n");
    } else {
        // int
        fprintf(out,"move $a0, %s\n", valReg);
        fprintf(out,"li $v0, 1\n");
        fprintf(out,"syscall\n");
    }
    // newline
    fprintf(out,"li $a0, '\\n'\n");
    fprintf(out,"li $v0, 11\n");
    fprintf(out,"syscall\n");
}

// Function prologue/epilogue
static void printFunctionPrologue(FILE* out, const char* funcName) {
    fprintf(out,"%s:\n",funcName);
    fprintf(out,"addi $sp, $sp, -8\n");
    fprintf(out,"sw $ra, 4($sp)\n");
    fprintf(out,"sw $fp, 0($sp)\n");
    fprintf(out,"move $fp, $sp\n");
}

static void printFunctionEpilogue(FILE* out) {
    fprintf(out,"move $sp, $fp\n");
    fprintf(out,"lw $fp, 0($sp)\n");
    fprintf(out,"lw $ra, 4($sp)\n");
    fprintf(out,"addi $sp, $sp, 8\n");
    fprintf(out,"jr $ra\n");
}

// Comparison for floats:
// float comparison uses: c.eq.s, c.lt.s, etc.
// then bc1t or bc1f to branch
// We'll produce a temporary boolean in an int register by setting it after branching.
static void doComparison(FILE* out,const char* op,const char* result,const char* arg1,const char* arg2) {
    SymbolType t1 = getSymbolType(arg1);
    SymbolType t2 = getSymbolType(arg2);
    // If one is float => float comparison
    SymbolType t = (t1==TYPE_FLOAT||t2==TYPE_FLOAT)?TYPE_FLOAT:TYPE_INT;
    char* rRes = (t==TYPE_FLOAT)?getIntRegForTemp(result):getIntRegForTemp(result);
    // We'll store comparison in an int reg (0 or 1)

    if (t==TYPE_FLOAT) {
        // load to $f0,$f1
        char* A1 = loadOperand(out,arg1); // $f0 or $t0
        if (getSymbolType(arg1)==TYPE_INT) {
            // move int to float register?
            // For simplicity, assume no int->float needed. If needed:
            // convert int to float??? This is complicated.
            // Let's assume all properly typed.
        }
        fprintf(out,"mov.s $f4, %s\n",A1);
        char* A2 = loadOperand(out,arg2);
        fprintf(out,"mov.s $f5, %s\n",A2);

        // Compare:
        if (!strcmp(op,"==")) {
            fprintf(out,"c.eq.s $f4, $f5\n");
        } else if(!strcmp(op,"!=")) {
            fprintf(out,"c.eq.s $f4, $f5\n");
        } else if(!strcmp(op,"<")) {
            fprintf(out,"c.lt.s $f4, $f5\n");
        } else if(!strcmp(op,"<=")) {
            fprintf(out,"c.le.s $f4, $f5\n");
        } else if(!strcmp(op,">")) {
            // c.lt.s $f5, $f4 => invert
            fprintf(out,"c.lt.s $f5, $f4\n");
        } else if(!strcmp(op,">=")) {
            // c.le.s $f5, $f4
            fprintf(out,"c.le.s $f5, $f4\n");
        }

        // bc1t means if condition true
        // We'll do:
        // li $tX, 0
        // bc1t L_true
        // li $tX, 1
        // j L_endcomp
        //L_true:
        // li $tX, 1
        //L_endcomp:

        static int compCount=0;
        char labelTrue[32], labelEnd[32];
        sprintf(labelTrue,"L_fcomptrue_%d",compCount);
        sprintf(labelEnd,"L_fcompend_%d",compCount);
        compCount++;

        fprintf(out,"li %s,0\n",rRes);

        if(!strcmp(op,"!=")) {
            // != is true if not eq
            // we did c.eq.s, so bc1t => equal => set 0 else 1
            fprintf(out,"bc1f %s\n",labelTrue);
        } else if(!strcmp(op,">")) {
            // c.lt.s $f5,$f4 means $f5<$f4 if true => $f4>$f5
            fprintf(out,"bc1t %s\n",labelTrue);
        } else if(!strcmp(op,">=")) {
            // c.le.s $f5,$f4 means $f5<=$f4 if true => $f4>=$f5
            fprintf(out,"bc1t %s\n",labelTrue);
        } else {
            // For ==, <, <= we used direct conditions
            fprintf(out,"bc1t %s\n",labelTrue);
            if (!strcmp(op,"==")) {
                // eq.s true means equal => set rRes=1
            } else if(!strcmp(op,"<")) {
                // c.lt.s true => $f4<$f5 => correct
            } else if(!strcmp(op,"<=")) {
                // c.le.s true => $f4<=$f5
            }
        }

        // If we get here, no branch taken
        fprintf(out,"j %s\n",labelEnd);
        fprintf(out,"%s:\n",labelTrue);
        if(!strcmp(op,"!=")) {
            // eq means sets true if equal, we want not equal => bc1f means eq test was false => not eq => 1
            fprintf(out,"li %s,1\n",rRes);
        } else {
            fprintf(out,"li %s,1\n",rRes);
        }
        fprintf(out,"%s:\n",labelEnd);

    } else {
        // int comparison: use pseudo instructions
        char* A1=loadOperand(out,arg1);
        char* A2=loadOperand(out,arg2);
        if (!strcmp(op,"==")) fprintf(out,"seq %s, %s, %s\n",rRes,A1,A2);
        else if(!strcmp(op,"!=")) fprintf(out,"sne %s, %s, %s\n",rRes,A1,A2);
        else if(!strcmp(op,"<")) fprintf(out,"slt %s, %s, %s\n",rRes,A1,A2);
        else if(!strcmp(op,"<=")) fprintf(out,"sle %s, %s, %s\n",rRes,A1,A2);
        else if(!strcmp(op,">")) fprintf(out,"sgt %s, %s, %s\n",rRes,A1,A2);
        else if(!strcmp(op,">=")) fprintf(out,"sge %s, %s, %s\n",rRes,A1,A2);
    }
}

static void printTextSection(FILE* out, TAC* tac) {
    fprintf(out,".text\n");
    fprintf(out,".globl main\n");
    fprintf(out,"main:\n");

    for (TAC* c=tac;c;c=c->next) {
        if (c->operator) {
            if (!strcmp(c->operator,"label")) {
                // Check if it's func_ start or endfunc_
                if (!strncmp(c->result,"func_",5)) {
                    printFunctionPrologue(out,c->result);
                } else if(!strncmp(c->result,"endfunc_",8)) {
                    printFunctionEpilogue(out);
                } else {
                    fprintf(out,"%s:\n",c->result);
                }
            } else if(!strcmp(c->operator,"=")) {
                // result = arg1
                SymbolType t = getSymbolType(c->result);
                char* valReg = loadOperand(out,c->arg1);
                storeValue(out,c->result,valReg);
            } else if(!strcmp(c->operator,"+")||!strcmp(c->operator,"-")||!strcmp(c->operator,"*")||!strcmp(c->operator,"/")) {
                SymbolType rt = getSymbolType(c->result);
                char* rRes = (rt==TYPE_FLOAT)?getFloatRegForTemp(c->result):getIntRegForTemp(c->result);
                char* A1 = loadOperand(out,c->arg1);
                char* A2 = loadOperand(out,c->arg2);
                if (rt==TYPE_FLOAT) {
                    if(!strcmp(c->operator,"+")) fprintf(out,"add.s %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"-")) fprintf(out,"sub.s %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"*")) fprintf(out,"mul.s %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"/")) fprintf(out,"div.s %s, %s, %s\n",rRes,A1,A2);
                } else {
                    if(!strcmp(c->operator,"+")) fprintf(out,"add %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"-")) fprintf(out,"sub %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"*")) fprintf(out,"mul %s, %s, %s\n",rRes,A1,A2);
                    else if(!strcmp(c->operator,"/")) {
                        fprintf(out,"div %s, %s\n",A1,A2);
                        fprintf(out,"mflo %s\n",rRes);
                    }
                }
            } else if(!strcmp(c->operator,"write")) {
                printValue(out,c->arg1);
            } else if(!strcmp(c->operator,"load")) {
                // result = load arr[arg2], arg1=arrName
                loadArrayElement(out,c->arg1,c->arg2);
                // now in $f0 or $t0
                if (isTemp(c->result)) {
                    // It's already in correct reg
                } else {
                    // store to var
                    SymbolType t = getSymbolType(c->arg1); // array type
                    if (t==TYPE_FLOAT) {
                        fprintf(out,"swc1 $f0, %s\n", c->result);
                    } else {
                        fprintf(out,"sw $t0, %s\n", c->result);
                    }
                }
            } else if(!strcmp(c->operator,"store")) {
                // store arg1, result[arg2]
                storeArrayElement(out,c->result,c->arg2,c->arg1);
            } else if(!strcmp(c->operator,"if")) {
                // if arg1 !=0 goto result
                // load arg1
                char* cond = loadOperand(out,c->arg1);
                fprintf(out,"bne %s, $zero, %s\n",cond,c->result);
            } else if(!strcmp(c->operator,"goto")) {
                fprintf(out,"j %s\n",c->result);
            } else if(!strcmp(c->operator,"call")) {
                // call func
                fprintf(out,"jal %s\n",c->arg1);
            } else if(!strcmp(c->operator,"==")||!strcmp(c->operator,"!=")||!strcmp(c->operator,"<")||
                      !strcmp(c->operator,"<=")||!strcmp(c->operator,">")||!strcmp(c->operator,">=")) {
                doComparison(out,c->operator,c->result,c->arg1,c->arg2);
            }
        }
    }

    // Exit program
    fprintf(out,"li $v0, 10\n");
    fprintf(out,"syscall\n");
}

// main function
extern SymbolTable* currentScope;
extern ScopeNode* scopeListHead;

void generateMIPS(TAC* tacHead, const char* outputFilename) {
    FILE* out = fopen(outputFilename,"w");
    if (!out) {
        perror("Failed to open output file");
        return;
    }

    // Print data section from symbol table info
    printDataSection(out);

    // Print text section
    printTextSection(out, tacHead);

    fclose(out);
}

*/