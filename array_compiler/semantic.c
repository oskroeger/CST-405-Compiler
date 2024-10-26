#include "semantic.h"
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

// Define global variables
TAC* tacHead = NULL;
TAC* tacTail = NULL;
int tempVarFloatCounter = 0;
int tempVarIntCounter = 0;

char* generateTempIntVar() {
    char* tempVar = (char*)malloc(10 * sizeof(char));
    if (!tempVar) {
        fprintf(stderr, "Memory allocation failed for tempVar.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(tempVar, "t%d", tempVarIntCounter++);
    return tempVar;
}

char* generateTempFloatVar() {
    char* tempVar = (char*)malloc(10 * sizeof(char));
    if (!tempVar) {
        fprintf(stderr, "Memory allocation failed for tempVar.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(tempVar, "f%d", tempVarFloatCounter++);
    return tempVar;
}


void generateTAC(const char* operation, const char* result, const char* operand1, const char* operand2) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    if (newTAC == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for TAC.\n");
        exit(EXIT_FAILURE);
    }

    // Check if the result is an array access and format it accordingly
    if (result && strchr(result, '[')) {
        // Array assignment: Use the array access (e.g., arr[0])
        newTAC->result = strdup(result);
    } else {
        newTAC->result = result ? strdup(result) : NULL;
    }

    newTAC->operation = strdup(operation);

    // Set operand1 and operand2 (if provided)
    newTAC->operand1 = operand1 ? strdup(operand1) : NULL;
    newTAC->operand2 = operand2 ? strdup(operand2) : NULL;

    newTAC->next = NULL;

    // Add the new TAC instruction to the list
    if (tacTail == NULL) {
        tacHead = newTAC;
    } else {
        tacTail->next = newTAC;
    }
    tacTail = newTAC;
}


void printTAC() {
    TAC* current = tacHead;
    while (current != NULL) {
        if (current->operand2) {
            // For binary operations like ADD, SUB, etc.
            printf("%s = %s %s %s\n", current->result, current->operand1, current->operation, current->operand2);
        } else if (current->operand1) {
            // For unary operations like MOV
            // Always include the operation even for array accesses
            printf("%s = %s %s\n", current->result, current->operation, current->operand1);
        } else {
            // For operations that only involve the result (like return statements)
            printf("%s = %s\n", current->result, current->operation);
        }
        current = current->next;
    }
}


char* generateExprTAC(ASTNode* expr, SymbolTable* symTab) {
    if (expr->type == NodeType_IntExpr) {
        char* tempVar = generateTempIntVar();
        char valueStr[20];
        sprintf(valueStr, "%d", expr->IntExpr.integer);
        generateTAC("MOV", tempVar, valueStr, NULL);
        return tempVar;
    } else if (expr->type == NodeType_FloatExpr) {
        char* tempVar = generateTempFloatVar();
        char valueStr[20];
        sprintf(valueStr, "%f", expr->FloatExpr.floatNum);
        generateTAC("MOV", tempVar, valueStr, NULL);
        return tempVar;
    } else if (expr->type == NodeType_SimpleID) {
        Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
        if (sym != NULL) {
            return sym->name;  // Return the variable name
        } else {
            fprintf(stderr, "Error: Variable %s used before declaration.\n", expr->simpleID.name);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_ArrayAccess) {
        // Lookup the array in the symbol table
        Symbol* sym = lookupSymbol(symTab, expr->arrayAccess.arrayName);
        if (sym == NULL) {
            fprintf(stderr, "Error: Array %s used before declaration.\n", expr->arrayAccess.arrayName);
            exit(EXIT_FAILURE);
        }

        // Check if the index is a constant integer expression
        char* indexStr;
        if (expr->arrayAccess.index->type == NodeType_IntExpr) {
            // If the index is a constant integer, use it directly
            indexStr = (char*)malloc(10 * sizeof(char));
            sprintf(indexStr, "%d", expr->arrayAccess.index->IntExpr.integer);  // Directly use the constant index
        } else {
            // Otherwise, evaluate the index expression (if it's a variable or more complex expression)
            indexStr = generateExprTAC(expr->arrayAccess.index, symTab);
        }

        // Return the array element reference directly, e.g., "arr[0]" or "arr[i]"
        char* arrayAccessStr = (char*)malloc(50 * sizeof(char));
        sprintf(arrayAccessStr, "%s[%s]", sym->name, indexStr);  // Format the array access as "arr[index]"
        return arrayAccessStr;  // Return the array access as a reference for use in TAC
    } else if (expr->type == NodeType_Expr) {
        char* leftVar = generateExprTAC(expr->expr.left, symTab);
        char* rightVar = generateExprTAC(expr->expr.right, symTab);

        SymbolType leftType = determineExprType(expr->expr.left, symTab);
        SymbolType rightType = determineExprType(expr->expr.right, symTab);

        char* tempVar = NULL;

        // Handle type promotion if necessary
        if (leftType == TYPE_INT && rightType == TYPE_FLOAT) {
            char* promotedLeft = generateTempFloatVar();
            generateTAC("CAST_TO_FLOAT", promotedLeft, leftVar, NULL);
            leftVar = promotedLeft;
            tempVar = generateTempFloatVar();
        } else if (leftType == TYPE_FLOAT && rightType == TYPE_INT) {
            char* promotedRight = generateTempFloatVar();
            generateTAC("CAST_TO_FLOAT", promotedRight, rightVar, NULL);
            rightVar = promotedRight;
            tempVar = generateTempFloatVar();
        } else if (leftType == TYPE_INT && rightType == TYPE_INT) {
            tempVar = generateTempIntVar();
        } else if (leftType == TYPE_FLOAT && rightType == TYPE_FLOAT) {
            tempVar = generateTempFloatVar();
        }

        // Handle the operators
        if (strcmp(expr->expr.operator, "+") == 0) {
            generateTAC("ADD", tempVar, leftVar, rightVar);
        } else if (strcmp(expr->expr.operator, "-") == 0) {
            generateTAC("SUB", tempVar, leftVar, rightVar);
        } else if (strcmp(expr->expr.operator, "*") == 0) {
            generateTAC("MUL", tempVar, leftVar, rightVar);
        } else if (strcmp(expr->expr.operator, "/") == 0) {
            generateTAC("DIV", tempVar, leftVar, rightVar);
        }

        return tempVar;
    }

    return NULL;
}


SymbolType determineExprType(ASTNode* expr, SymbolTable* symTab) {
    if (!expr) return TYPE_UNKNOWN;

    switch (expr->type) {
        case NodeType_IntExpr:
            return TYPE_INT;  // Integer expression
        case NodeType_FloatExpr:
            return TYPE_FLOAT;  // Float expression
        case NodeType_SimpleID: {
            Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
            if (sym) {
                return sym->type;  // Return the variable's type
            } else {
                fprintf(stderr, "Semantic Error: Variable %s used before declaration.\n", expr->simpleID.name);
                exit(EXIT_FAILURE);
            }
        }
        case NodeType_ArrayAccess: {
            Symbol* sym = lookupSymbol(symTab, expr->arrayAccess.arrayName);
            if (sym) {
                return sym->type;  // Return the array's type
            } else {
                fprintf(stderr, "Semantic Error: Array %s used before declaration.\n", expr->arrayAccess.arrayName);
                exit(EXIT_FAILURE);
            }
        }
        case NodeType_Expr:
            return determineExprType(expr->expr.left, symTab);  // Binary expression
        default:
            return TYPE_UNKNOWN;  // Unknown type for unhandled cases
    }
}

void semanticCheck(ASTNode* node, SymbolTable* symTab) {
    if (!node) return;

    switch (node->type) {
        case NodeType_Program:
            printf("Checking the program...\n");
            semanticCheck(node->program.varDeclList, symTab);
            semanticCheck(node->program.stmtList, symTab);
            break;

        case NodeType_VarDeclList:
            semanticCheck(node->varDeclList.varDecl, symTab);
            semanticCheck(node->varDeclList.varDeclList, symTab);
            break;

        case NodeType_VarDecl:
            printf("Checking variable '%s' of type '%s'...\n", node->varDecl.varName, node->varDecl.varType);
            break;

        case NodeType_ArrayDecl:
            printf("Checking array '%s' of type '%s' with size %d...\n", 
                   node->arrayDecl.varName, node->arrayDecl.varType, node->arrayDecl.size);
            break;

        case NodeType_IntExpr:
            printf("Integer expression with value: %d\n", node->IntExpr.integer);
            break;

        case NodeType_FloatExpr:
            printf("Float expression with value: %f\n", node->FloatExpr.floatNum);
            break;

        case NodeType_SimpleID: {
            printf("Looking up variable '%s'...\n", node->simpleID.name);
            Symbol* sym = lookupSymbol(symTab, node->simpleID.name);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->simpleID.name);
                exit(EXIT_FAILURE);
            }
            break;
        }

        case NodeType_ArrayAccess: {
            printf("Checking array access for variable '%s'...\n", node->arrayAccess.arrayName);
            Symbol* sym = lookupSymbol(symTab, node->arrayAccess.arrayName);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Array '%s' used before declaration.\n", node->arrayAccess.arrayName);
                exit(EXIT_FAILURE);
            }
            semanticCheck(node->arrayAccess.index, symTab);  // Check the index expression for validity
            break;
        }

        case NodeType_Expr: {
            printf("Checking expression with operator '%s'...\n", node->expr.operator);
            semanticCheck(node->expr.left, symTab);  // Recursively check left and right sides
            semanticCheck(node->expr.right, symTab);

            SymbolType leftType = determineExprType(node->expr.left, symTab);
            SymbolType rightType = determineExprType(node->expr.right, symTab);
            if (leftType != rightType) {
                fprintf(stderr, "Semantic Error: Type mismatch in expression with operator '%s'. Left type: %s, Right type: %s.\n",
                        node->expr.operator, symbolTypeToString(leftType), symbolTypeToString(rightType));
                exit(EXIT_FAILURE);
            }
            break;
        }

        case NodeType_StmtList:
            semanticCheck(node->stmtList.stmt, symTab);
            semanticCheck(node->stmtList.stmtList, symTab);
            break;

        case NodeType_WriteStmt: {
            printf("Checking write statement for variable '%s'...\n", node->writeStmt.id);
            Symbol* sym = lookupSymbol(symTab, node->writeStmt.id);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->writeStmt.id);
                exit(EXIT_FAILURE);
            }
            break;
        }

        case NodeType_AssignStmt: {
            printf("Checking assignment to variable '%s'...\n", node->assignStmt.varName);
            Symbol* sym = lookupSymbol(symTab, node->assignStmt.varName);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->assignStmt.varName);
                exit(EXIT_FAILURE);
            }

            SymbolType exprType = determineExprType(node->assignStmt.expr, symTab);
            if (sym->type != exprType) {
                fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'. Expected: %s, Found: %s.\n",
                        node->assignStmt.varName, symbolTypeToString(sym->type), symbolTypeToString(exprType));
                exit(EXIT_FAILURE);
            }

            semanticCheck(node->assignStmt.expr, symTab);  // Check the assigned expression
            break;
        }

        default:
            fprintf(stderr, "Semantic Error: Unknown node type.\n");
            exit(EXIT_FAILURE);
    }
}

void checkSemantics(ASTNode* root, SymbolTable* symTab) {
    semanticCheck(root, symTab);
}
