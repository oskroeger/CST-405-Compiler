#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// A helper function to generate unique temporary names.
// (Optional improvement: you can maintain a global counter.)
static int tempCounter = 0;
static char* newTemp() {
    char* buffer = (char*)malloc(20);
    sprintf(buffer, "t%d", tempCounter++);
    return buffer;
}

// Helper to create a TAC node
TAC* createTAC(char* operator, char* arg1, char* arg2, char* result) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    newTAC->operator = operator ? strdup(operator) : NULL;
    newTAC->arg1 = arg1 ? strdup(arg1) : NULL;
    newTAC->arg2 = arg2 ? strdup(arg2) : NULL;
    newTAC->result = result ? strdup(result) : NULL;
    newTAC->next = NULL;
    return newTAC;
}

// Helper to append TAC to a list
TAC* appendTAC(TAC* head, TAC* newTAC) {
    if (!head) return newTAC;
    TAC* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newTAC;
    return head;
}

TAC* generateTAC(ASTNode* node, char* target) {
    if (!node) return NULL;
    TAC* code = NULL;

    switch (node->type) {
        case NodeType_Program:
            // Traverse children
            code = appendTAC(code, generateTAC(node->program.functionDefList, NULL));
            code = appendTAC(code, generateTAC(node->program.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->program.stmtList, NULL));
            break;

        case NodeType_FunctionDefList:
            // Generate TAC for function definitions
            code = appendTAC(code, generateTAC(node->functionDefList.functionDef, NULL));
            code = appendTAC(code, generateTAC(node->functionDefList.functionDefList, NULL));
            break;

        case NodeType_FunctionDef: {
            char funcLabel[50];
            char endFuncLabel[50];
            sprintf(funcLabel, "func_%s", node->functionDef.name);
            sprintf(endFuncLabel, "endfunc_%s", node->functionDef.name);

            // Function start label
            code = appendTAC(code, createTAC("label", NULL, NULL, funcLabel));
            // Generate TAC for varDeclList and stmtList
            code = appendTAC(code, generateTAC(node->functionDef.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->functionDef.stmtList, NULL));
            // Function end label
            code = appendTAC(code, createTAC("label", NULL, NULL, endFuncLabel));
            break;
        }

        case NodeType_StmtList:
            code = appendTAC(code, generateTAC(node->stmtList.stmt, NULL));
            code = appendTAC(code, generateTAC(node->stmtList.stmtList, NULL));
            break;

        case NodeType_AssignStmt: {
            if (node->assignStmt.isArray) {
                // Handle array assignment
                char* indexTemp = newTemp();
                TAC* code = generateTAC(node->assignStmt.arrayIndex, indexTemp);
                // Evaluate the RHS
                char* rhsTemp = newTemp();
                code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTemp));
                // Generate store instruction
                code = appendTAC(code, createTAC("store", rhsTemp, indexTemp, node->assignStmt.varName));
                return code;
            } else {
                // Normal variable assignment
                char* rhsTarget = target ? strdup(target) : newTemp();
                TAC* code = generateTAC(node->assignStmt.expr, rhsTarget);
                code = appendTAC(code, createTAC("=", rhsTarget, NULL, node->assignStmt.varName));
                return code;
            }
        }

        case NodeType_Expr: {
            // If it's a comparison operator, we need to evaluate it into a boolean (0/1)
            // Let's assume node->expr.operator is the actual operator from the code (==, !=, <, etc.)
            // We'll produce tCond = (tL operator tR), resulting in 0 or 1. Then the IfStmt uses that tCond.
            // For arithmetic ops, just do arithmetic.

            char* lhsTemp = newTemp();
            char* rhsTemp = newTemp();
            code = appendTAC(code, generateTAC(node->expr.left, lhsTemp));
            code = appendTAC(code, generateTAC(node->expr.right, rhsTemp));

            char* resultTemp = target ? target : newTemp();
            // Check if operator is comparison:
            if (strcmp(node->expr.operator, "==") == 0 ||
                strcmp(node->expr.operator, "!=") == 0 ||
                strcmp(node->expr.operator, "<") == 0 ||
                strcmp(node->expr.operator, "<=") == 0 ||
                strcmp(node->expr.operator, ">") == 0 ||
                strcmp(node->expr.operator, ">=") == 0) {
                // Generate a comparison instruction:
                // We'll just do resultTemp = lhsTemp operator rhsTemp (1 or 0)
                // In a real compiler, you'd need a separate instruction to set a boolean result.
                code = appendTAC(code, createTAC(node->expr.operator, lhsTemp, rhsTemp, resultTemp));
            } else {
                // Arithmetic operator
                code = appendTAC(code, createTAC(node->expr.operator, lhsTemp, rhsTemp, resultTemp));
            }
            break;
        }

        case NodeType_IntExpr: {
            char* resultTemp = target ? target : newTemp();
            TAC* instr = createTAC("=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%d", node->IntExpr.integer);
            code = appendTAC(code, instr);
            break;
        }

        case NodeType_FloatExpr: {
            char* resultTemp = target ? target : newTemp();
            TAC* instr = createTAC("=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%f", node->FloatExpr.floatNum);
            code = appendTAC(code, instr);
            break;
        }

        case NodeType_SimpleID: {
            char* resultTemp = target ? target : newTemp();
            code = appendTAC(code, createTAC("load", node->simpleID.name, NULL, resultTemp));
            break;
        }

        case NodeType_ArrayAccess: {
            char* indexTemp = newTemp();
            code = appendTAC(code, generateTAC(node->arrayAccess.index, indexTemp));
            char* resultTemp = target ? target : newTemp();
            code = appendTAC(code, createTAC("load", node->arrayAccess.arrayName, indexTemp, resultTemp));
            break;
        }

        case NodeType_WriteStmt:
            code = appendTAC(code, createTAC("write", node->writeStmt.id, NULL, NULL));
            break;

        case NodeType_IfStmt: {
            char* condTemp = newTemp();
            code = appendTAC(code, generateTAC(node->ifStmt.condition, condTemp));

            char* labelThen = strdup("L_then");
            // char* labelElse = strdup("L_else");
            char* labelEnd = strdup("L_end");

            // if condTemp == 1 goto L_then
            // We'll assume condTemp holds a boolean result (1/0).
            // In a real compiler, you might need a special instruction. For simplicity:
            code = appendTAC(code, createTAC("if", condTemp, NULL, labelThen));

            // else part
            if (node->ifStmt.elseStmt) {
                code = appendTAC(code, generateTAC(node->ifStmt.elseStmt, NULL));
            }
            code = appendTAC(code, createTAC("goto", NULL, NULL, labelEnd));

            code = appendTAC(code, createTAC("label", NULL, NULL, labelThen));
            code = appendTAC(code, generateTAC(node->ifStmt.thenStmt, NULL));

            code = appendTAC(code, createTAC("label", NULL, NULL, labelEnd));
            break;
        }

        case NodeType_VarDeclList:
            code = appendTAC(code, generateTAC(node->varDeclList.varDecl, NULL));
            code = appendTAC(code, generateTAC(node->varDeclList.varDeclList, NULL));
            break;

        case NodeType_VarDecl:
        case NodeType_ArrayDecl:
            // No TAC for variable/array declarations (just skip)
            break;

        case NodeType_FunctionCall:
            // call funcName
            code = appendTAC(code, createTAC("call", node->functionCall.name, NULL, NULL));
            break;

        default:
            printf("[DEBUG] No TAC generation for node type: %s\n", nodeTypeToString(node->type));
            break;
    }

    return code;
}


// Print the TAC
void printTAC(TAC* tac) {
    TAC* current = tac;
    while (current) {
        if (current->operator && strcmp(current->operator, "=") == 0) {
            // Assignment: result = arg1
            printf("%s = %s\n", current->result, current->arg1);
        } else if (current->operator && 
                  (strcmp(current->operator, "+") == 0 ||
                   strcmp(current->operator, "-") == 0 ||
                   strcmp(current->operator, "*") == 0 ||
                   strcmp(current->operator, "/") == 0)) {
            // Binary operation: result = arg1 operator arg2
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && 
                   (strcmp(current->operator, "==") == 0 ||
                    strcmp(current->operator, "!=") == 0 ||
                    strcmp(current->operator, "<") == 0 ||
                    strcmp(current->operator, "<=") == 0 ||
                    strcmp(current->operator, ">") == 0 ||
                    strcmp(current->operator, ">=") == 0)) {
            // Comparison: result = arg1 op arg2
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && strcmp(current->operator, "write") == 0) {
            printf("write %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator, "load") == 0) {
            if (current->arg2) {
                printf("%s = load %s[%s]\n", current->result, current->arg1, current->arg2);
            } else {
                printf("%s = load %s\n", current->result, current->arg1);
            }
        } else if (current->operator && strcmp(current->operator, "if") == 0) {
            printf("if %s goto %s\n", current->arg1, current->result);
        } else if (current->operator && strcmp(current->operator, "goto") == 0) {
            printf("goto %s\n", current->result);
        } else if (current->operator && strcmp(current->operator, "label") == 0) {
            printf("%s:\n", current->result);
        } else if (current->operator && strcmp(current->operator, "call") == 0) {
            printf("call %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator, "store") == 0) {
            // store arg1 (value), result[arg2]
            printf("store %s, %s[%s]\n", current->arg1, current->result, current->arg2);
        } else {
            // Fallback
            printf("%s = %s %s %s\n",
                   current->result ? current->result : "",
                   current->arg1 ? current->arg1 : "",
                   current->operator ? current->operator : "",
                   current->arg2 ? current->arg2 : "");
        }

        current = current->next;
    }
}

// Free the TAC linked list
void freeTAC(TAC* tac) {
    while (tac) {
        TAC* temp = tac;
        tac = tac->next;
        if (temp->operator) free(temp->operator);
        if (temp->arg1) free(temp->arg1);
        if (temp->arg2) free(temp->arg2);
        if (temp->result) free(temp->result);
        free(temp);
    }
}
