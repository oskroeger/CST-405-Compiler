#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// A helper function to generate unique temporary names.
static int tempCounter = 0;
static int floatCounter = 0;
static char* newTemp(DataType type) {
    char* buffer = (char*)malloc(20);
    if (type == TYPE_F) {
        sprintf(buffer, "f_%d", floatCounter++);
    } else{
        sprintf(buffer, "t_%d", tempCounter++);
    }
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

// Forward declaration for handling arguments in function calls
static TAC* generateArgs(ASTNode* args);

TAC* generateTAC(ASTNode* node, char* target) {
    if (!node) return NULL;
    TAC* code = NULL;

    switch (node->type) {
        case NodeType_Program:
            code = appendTAC(code, generateTAC(node->program.functionDefList, NULL));
            code = appendTAC(code, generateTAC(node->program.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->program.stmtList, NULL));
            break;

        case NodeType_FunctionDefList:
            code = appendTAC(code, generateTAC(node->functionDefList.functionDef, NULL));
            code = appendTAC(code, generateTAC(node->functionDefList.functionDefList, NULL));
            break;

        case NodeType_FunctionDef: {
            char funcLabel[50];
            char endFuncLabel[50];
            sprintf(funcLabel, "func_%s", node->functionDef.name);
            sprintf(endFuncLabel, "endfunc_%s", node->functionDef.name);

            code = appendTAC(code, createTAC("label", NULL, NULL, funcLabel));
            code = appendTAC(code, generateTAC(node->functionDef.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->functionDef.stmtList, NULL));
            code = appendTAC(code, createTAC("label", NULL, NULL, endFuncLabel));
            break;
        }

        case NodeType_StmtList:
            code = appendTAC(code, generateTAC(node->stmtList.stmt, NULL));
            code = appendTAC(code, generateTAC(node->stmtList.stmtList, NULL));
            break;

        case NodeType_AssignStmt: {
            if (node->assignStmt.isArray) {
                char* indexTemp = newTemp( node->assignStmt.arrayIndex->dataType);
                code = appendTAC(code, generateTAC(node->assignStmt.arrayIndex, indexTemp));
                char* rhsTemp = newTemp(node->assignStmt.expr->dataType);
                code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTemp));
                code = appendTAC(code, createTAC("store", rhsTemp, indexTemp, node->assignStmt.varName));
            } else {
                char* rhsTarget = target ? strdup(target) : newTemp(node->assignStmt.expr->dataType);

                // Check if the LHS is a float
                DataType lhsType = lookupTypeInSymbolTable(currentScope, node->assignStmt.varName);

                if (lhsType == TYPE_INT) {
                    code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTarget));
                    code = appendTAC(code, createTAC("=", rhsTarget, NULL, node->assignStmt.varName));
                }
                else if (lhsType == TYPE_FLOAT) {
                    code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTarget));
                    code = appendTAC(code, createTAC("f=", rhsTarget, NULL, node->assignStmt.varName));
                }

                
            }
            break;
        }

        case NodeType_Expr: {
            char* lhsTemp = newTemp(node->expr.left->dataType);
            char* rhsTemp = newTemp(node->expr.right->dataType);
            code = appendTAC(code, generateTAC(node->expr.left, lhsTemp));
            code = appendTAC(code, generateTAC(node->expr.right, rhsTemp));

            char* resultTemp = target ? target : newTemp(node->expr.left->dataType);
            code = appendTAC(code, createTAC(node->expr.operator, lhsTemp, rhsTemp, resultTemp));

            // check if it s a float operation
            if (node->expr.left->dataType == TYPE_F || node->expr.right->dataType == TYPE_F) {
                //change the operator to f operator

                printf ("[INFO] Generated TAC for expression: %s f= %s %s %s\n", resultTemp, lhsTemp, node->expr.operator, rhsTemp);

            } else {
                printf ("[INFO] Generated TAC for expression: %s = %s %s %s\n", resultTemp, lhsTemp, node->expr.operator, rhsTemp);
            }
            break;
        }

        case NodeType_IntExpr: {
            char* resultTemp = target ? target : newTemp(TYPE_I);
            TAC* instr = createTAC("=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%d", node->IntExpr.integer);
            code = appendTAC(code, instr);
            printf ("[INFO] Generated TAC for int expression: %s = %d\n", resultTemp, node->IntExpr.integer);
            break;
        }

        case NodeType_FloatExpr: {
            char* resultTemp = target ? target : newTemp(TYPE_F);
            TAC* instr = createTAC("f=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%f", node->FloatExpr.floatNum);
            code = appendTAC(code, instr);

            printf ("[INFO] Generated TAC for float expression: %s f= %f\n", resultTemp, node->FloatExpr.floatNum);
            break;
        }

        case NodeType_SimpleID: {
            char* resultTemp = target ? target : newTemp(TYPE_I);
            code = appendTAC(code, createTAC("load", node->simpleID.name, NULL, resultTemp));
            break;
        }

        case NodeType_ArrayAccess: {
            char* indexTemp = newTemp(node->arrayAccess.index->dataType);
            code = appendTAC(code, generateTAC(node->arrayAccess.index, indexTemp));
            char* resultTemp = target ? target : newTemp(TYPE_I);
            code = appendTAC(code, createTAC("load", node->arrayAccess.arrayName, indexTemp, resultTemp));
            break;
        }

        case NodeType_WriteStmt:
            // check teh type of the id first
            if (lookupTypeInSymbolTable(currentScope, node->writeStmt.id) == TYPE_INT) {
                code = appendTAC(code, createTAC("write", node->writeStmt.id, NULL, NULL));
            } else {
                code = appendTAC(code, createTAC("fwrite", node->writeStmt.id, NULL, NULL));
            }
            //code = appendTAC(code, createTAC("write", node->writeStmt.id, NULL, NULL));
            break;

        case NodeType_IfStmt: {
            char* condTemp = newTemp(node->ifStmt.condition->dataType);
            code = appendTAC(code, generateTAC(node->ifStmt.condition, condTemp));

            char* labelThen = strdup("L_then");
            char* labelEnd = strdup("L_end");
            code = appendTAC(code, createTAC("if", condTemp, NULL, labelThen));

            if (node->ifStmt.elseStmt) {
                code = appendTAC(code, generateTAC(node->ifStmt.elseStmt, NULL));
            }
            code = appendTAC(code, createTAC("goto", NULL, NULL, labelEnd));
            code = appendTAC(code, createTAC("label", NULL, NULL, labelThen));
            code = appendTAC(code, generateTAC(node->ifStmt.thenStmt, NULL));
            code = appendTAC(code, createTAC("label", NULL, NULL, labelEnd));
            break;
        }

        case NodeType_ReturnStmt: {
            char* retTemp = newTemp(node->returnStmt.expr->dataType);
            code = appendTAC(code, generateTAC(node->returnStmt.expr, retTemp));
            code = appendTAC(code, createTAC("return", retTemp, NULL, NULL));
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

        case NodeType_FunctionCall: {
            // Generate args
            TAC* argCode = NULL;
            ASTNode* argList = node->functionCall.args;
            int argCount=0;
            while (argList && argList->type == NodeType_StmtList) {
                ASTNode* singleArg = argList->stmtList.stmt;
                char* argTemp = newTemp(node->dataType);
                argCode = appendTAC(argCode, generateTAC(singleArg, argTemp));
                // param argTemp
                argCode = appendTAC(argCode, createTAC("param", argTemp, NULL, NULL));
                argList = argList->stmtList.stmtList;
                argCount++;
            }
            code = appendTAC(code, argCode);

            // If function returns a value in expressions, store in target
            char* resultTemp = target ? target : NULL; 
            // If resultTemp is NULL, means we don't need the return value
            code = appendTAC(code, createTAC("call", node->functionCall.name, NULL, resultTemp));

            break;
        }
        case NodeType_WhileStmt: {
            // Create meaningful labels for the start and end of the loop
            char* startLabel = (char*)malloc(20);
            char* endLabel = (char*)malloc(20);
            sprintf(startLabel, "L_while_start_%d", tempCounter++);
            sprintf(endLabel, "L_while_end_%d", tempCounter++);

            // Add a label for the start of the loop
            code = appendTAC(code, createTAC("label", NULL, NULL, startLabel));

            // Generate TAC for the loop condition
            char* condTemp = newTemp(node->whileStmt.condition->dataType);
            code = appendTAC(code, generateTAC(node->whileStmt.condition, condTemp));

            // Add a conditional jump to the end of the loop if the condition is false
            code = appendTAC(code, createTAC("ifFalse", condTemp, NULL, endLabel));

            // Generate TAC for the loop body
            TAC* bodyCode = generateTAC(node->whileStmt.body, NULL);
            if (bodyCode) {
                code = appendTAC(code, bodyCode);
            }

            // Add an unconditional jump back to the start of the loop
            code = appendTAC(code, createTAC("goto", NULL, NULL, startLabel));

            // Add a label for the end of the loop
            code = appendTAC(code, createTAC("label", NULL, NULL, endLabel));

            break;
        }

        default:
            printf("[DEBUG] No TAC generation for node type: %s\n", nodeTypeToString(node->type));
            break;
    }

    return code;
}


void printTAC(TAC* tac) {
    TAC* current = tac;
    while (current) {

        if (current->operator && strcmp(current->operator, "=") == 0) {
            printf("%s = %s\n", current->result, current->arg1);
        } else if (current->operator && strcmp(current->operator, "f=") == 0) {
            printf("%s f= %s\n", current->result, current->arg1);
        } else if (current->operator && strcmp(current->operator, "=") &&
                  (strcmp(current->operator, "+") == 0 ||
                   strcmp(current->operator, "-") == 0 ||
                   strcmp(current->operator, "*") == 0 ||
                   strcmp(current->operator, "/") == 0)) {
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && strcmp(current->operator, "f=") &&
                  (strcmp(current->operator, "f+") == 0 ||
                   strcmp(current->operator, "f-") == 0 ||
                   strcmp(current->operator, "f*") == 0 ||
                   strcmp(current->operator, "f/") == 0)) {
            printf("%s f= %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && strcmp(current->operator, "=") &&
                   (strcmp(current->operator, "==") == 0 ||
                    strcmp(current->operator, "!=") == 0 ||
                    strcmp(current->operator, "<") == 0 ||
                    strcmp(current->operator, "<=") == 0 ||
                    strcmp(current->operator, ">") == 0 ||
                    strcmp(current->operator, ">=") == 0)) {
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        }else if (current->operator && strcmp(current->operator, "f=") &&
                   (strcmp(current->operator, "f==") == 0 ||
                    strcmp(current->operator, "f!=") == 0 ||
                    strcmp(current->operator, "f<") == 0 ||
                    strcmp(current->operator, "f<=") == 0 ||
                    strcmp(current->operator, "f>") == 0 ||
                    strcmp(current->operator, "f>=") == 0)) {
            printf("%s f= %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && strcmp(current->operator, "write") == 0) {
            printf("write %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator, "fwrite") == 0) {
            printf("fwrite %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator, "load") == 0) {
            // detemine if its = or f=  based on the type of the result
            char* equals = (lookupTypeInSymbolTable(currentScope, current->arg1) == TYPE_FLOAT) ? "f=" : "=";

            if (current->arg2) {
                printf("%s %s load %s[%s]\n", current->result, equals, current->arg1, current->arg2);
            } else {
                printf("%s %s load %s\n", current->result, equals, current->arg1);
            }
        } else if (current->operator && strcmp(current->operator, "if") == 0) {
            printf("if %s goto %s\n", current->arg1, current->result);
        } else if (current->operator && strcmp(current->operator, "ifFalse") == 0) {
            printf("ifFalse %s goto %s // Exit loop if condition is false\n", current->arg1, current->result);
        } else if (current->operator && strcmp(current->operator, "goto") == 0) {
            printf("goto %s\n", current->result);
        } else if (current->operator && strcmp(current->operator, "label") == 0) {
            printf("%s:\n", current->result);
        } else if (current->operator && strcmp(current->operator, "call") == 0) {
            if (current->result) {
                printf("%s = call %s\n", current->result, current->arg1);
            } else {
                printf("call %s\n", current->arg1);
            }
        } else if (current->operator && strcmp(current->operator, "store") == 0) {
            printf("store %s, %s[%s]\n", current->arg1, current->result, current->arg2);
        } else if (current->operator && strcmp(current->operator,"return")==0) {
            printf("return %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator,"param")==0) {
            printf("param %s\n", current->arg1);
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