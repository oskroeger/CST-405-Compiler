#include "semantic.h"
#include <stdio.h>

// Define global variables
TAC* tacHead = NULL;
TAC* tacTail = NULL;
int tempVarCounter = 0;

char* generateTempVar() {
    char* tempVar = (char*)malloc(10 * sizeof(char));
    sprintf(tempVar, "t%d", tempVarCounter++);
    return tempVar;
}

char* generateExprTAC(ASTNode* expr, SymbolTable* symTab) {
    if (expr->type == NodeType_SimpleExpr) {
        char* tempVar = generateTempVar();
        char valueStr[20];
        sprintf(valueStr, "%d", expr->simpleExpr.number);
        generateTAC("MOV", tempVar, valueStr, NULL);  // Correct order
        return tempVar;
    } else if (expr->type == NodeType_SimpleID) {
        Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
        if (sym != NULL) {
            return sym->name;  // Return the variable name directly
        } else {
            fprintf(stderr, "Error: Variable %s used before declaration.\n", expr->simpleID.name);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_Expr) {
        char* leftVar = generateExprTAC(expr->expr.left, symTab);
        char* rightVar = generateExprTAC(expr->expr.right, symTab);
        char* tempVar = generateTempVar();

        if (strcmp(expr->expr.operator, "+") == 0) {
            generateTAC("ADD", tempVar, leftVar, rightVar);  // Correct order
            return tempVar;
        }
        // Handle other operators (e.g., -, *, /) as needed
    }
    return NULL; // Default return value if TAC generation fails
}

void generateTAC(const char* operation, const char* result, const char* operand1, const char* operand2) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    if (newTAC == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for TAC.\n");
        exit(EXIT_FAILURE);
    }
    newTAC->operation = strdup(operation);
    newTAC->result = strdup(result);
    newTAC->operand1 = operand1 ? strdup(operand1) : NULL;
    newTAC->operand2 = operand2 ? strdup(operand2) : NULL;
    newTAC->next = NULL;

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
        printf("%s = %s %s %s\n", current->result, current->operand1, current->operation, current->operand2 ? current->operand2 : "");
        current = current->next;
    }
}


char* determineExprType(ASTNode* expr, SymbolTable* symTab) {
    if (!expr) return NULL;

    switch (expr->type) {
        case NodeType_SimpleExpr:
            return "int";  // Assuming the expression is an integer
        case NodeType_SimpleID: {
            Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
            if (sym) return sym->type;  // Return the type of the variable
            else {
                fprintf(stderr, "Semantic Error: Variable %s used before declaration.\n", expr->simpleID.name);
                exit(EXIT_FAILURE);
            }
        }
        case NodeType_Expr:
            // Assuming binary operations only between same types (e.g., int + int)
            // You can extend this logic for more complex types
            return determineExprType(expr->expr.left, symTab);
        // Add more cases as needed for other node types
        default:
            return NULL;  // Unknown type
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
            printf("Checking variable declaration list...\n");
            semanticCheck(node->varDeclList.varDecl, symTab);
            semanticCheck(node->varDeclList.varDeclList, symTab);
            break;

        case NodeType_VarDecl:
            // Add the variable to the symbol table with its type
            printf("Adding variable '%s' of type '%s' to the symbol table.\n", node->varDecl.varName, node->varDecl.varType);
            break;

        case NodeType_SimpleExpr:
            // Simple expression (e.g., a number) typically doesn't need checking
            printf("Simple expression with value: %d\n", node->simpleExpr.number);
            break;

        case NodeType_SimpleID: {
            // Ensure the variable is declared
            printf("Looking up variable '%s'...\n", node->simpleID.name);
            Symbol* sym = lookupSymbol(symTab, node->simpleID.name);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->simpleID.name);
                exit(EXIT_FAILURE);
            }
            printf("Variable '%s' found (Type: %s).\n", sym->name, sym->type);
            break;
        }

        case NodeType_Expr:
            // Recursively check the left and right sides of the expression
            printf("Checking expression with operator '%s'...\n", node->expr.operator);
            semanticCheck(node->expr.left, symTab);
            semanticCheck(node->expr.right, symTab);
            break;

        case NodeType_StmtList:
            printf("Checking statement list...\n");
            semanticCheck(node->stmtList.stmt, symTab);
            semanticCheck(node->stmtList.stmtList, symTab);
            break;

        case NodeType_WriteStmt: {
            // Ensure the variable being written is declared
            printf("Checking write statement for variable '%s'...\n", node->writeStmt.id);
            Symbol* sym = lookupSymbol(symTab, node->writeStmt.id);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->writeStmt.id);
                exit(EXIT_FAILURE);
            }
            printf("Write statement valid for variable '%s' (Type: %s).\n", sym->name, sym->type);
            break;
        }

        case NodeType_AssignStmt: {
            // Ensure the variable being assigned to is declared
            printf("Checking assignment to variable '%s'...\n", node->assignStmt.varName);
            Symbol* sym = lookupSymbol(symTab, node->assignStmt.varName);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->assignStmt.varName);
                exit(EXIT_FAILURE);
            }
            printf("Variable '%s' found for assignment (Type: %s).\n", sym->name, sym->type);

            // Perform type checking
            char* exprType = determineExprType(node->assignStmt.expr, symTab);
            if (strcmp(sym->type, exprType) != 0) {
                fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'. Expected: %s, Found: %s.\n",
                        node->assignStmt.varName, sym->type, exprType);
                exit(EXIT_FAILURE);
            }
            printf("Type check passed for assignment to '%s' (Type: %s).\n", node->assignStmt.varName, sym->type);

            // Recursively check the expression
            semanticCheck(node->assignStmt.expr, symTab);
            break;
        }

        case NodeType_BinOp:
            // Recursively check the left and right operands of the binary operation
            printf("Checking binary operation '%s'...\n", node->binOp.operator);
            semanticCheck(node->binOp.left, symTab);
            semanticCheck(node->binOp.right, symTab);

            // Perform type checking for binary operations (e.g., both operands should be integers)
            char* leftType = determineExprType(node->binOp.left, symTab);
            char* rightType = determineExprType(node->binOp.right, symTab);
            if (strcmp(leftType, rightType) != 0) {
                fprintf(stderr, "Semantic Error: Type mismatch in binary operation '%s'. Left type: %s, Right type: %s.\n",
                        node->binOp.operator, leftType, rightType);
                exit(EXIT_FAILURE);
            }
            printf("Type check passed for binary operation '%s'.\n", node->binOp.operator);
            break;

        default:
            fprintf(stderr, "Semantic Error: Unknown node type.\n");
            exit(EXIT_FAILURE);
    }
}


void checkSemantics(ASTNode* root, SymbolTable* symTab) {
    semanticCheck(root, symTab);
}