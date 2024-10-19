#include "semantic.h"
#include <stdio.h>

// Define global variables
TAC* tacHead = NULL;
TAC* tacTail = NULL;
int tempVarFloatCounter = 0;
int tempVarIntCounter = 0;

char* generateTempIntVar() {
    char* tempVar = (char*)malloc(10 * sizeof(char));
    sprintf(tempVar, "t%d", tempVarIntCounter++);
    return tempVar;
}
char* generateTempFloatVar() {
    char* tempVar = (char*)malloc(10 * sizeof(char));
    sprintf(tempVar, "f%d", tempVarFloatCounter++);
    return tempVar;
}

char* generateExprTAC(ASTNode* expr, SymbolTable* symTab) {
    if (expr->type == NodeType_IntExpr) {
        // Handle simple expressions (constants)
        char* tempVar = generateTempIntVar();
        char valueStr[20];
        sprintf(valueStr, "%d", expr->IntExpr.integer);
        generateTAC("MOV", tempVar, valueStr, NULL);  // Move constant into temp variable
        return tempVar;
    } else if (expr->type == NodeType_FloatExpr) {
        // Handle simple expressions (constants)
        char* tempVar = generateTempFloatVar();
        char valueStr[20];
        sprintf(valueStr, "%f", expr->FloatExpr.floatNum);
        generateTAC("MOV", tempVar, valueStr, NULL);  // Move constant into temp variable
        return tempVar;
     } else if (expr->type == NodeType_SimpleID) {
        // Handle variable references
        Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
        if (sym != NULL) {
            return sym->name;  // Return the variable name directly
        } else {
            fprintf(stderr, "Error: Variable %s used before declaration.\n", expr->simpleID.name);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_Expr) {
        // Recursively generate TAC for left and right sub-expressions
        char* leftVar = generateExprTAC(expr->expr.left, symTab);
        char* rightVar = generateExprTAC(expr->expr.right, symTab);
        // if either left or right is a float, the result should be a float
        if (strcmp(determineExprType(expr->expr.left, symTab), "float") == 0 || strcmp(determineExprType(expr->expr.right, symTab), "float") == 0) {
            char* tempVar = generateTempFloatVar();

            // Handle the different operators
            if (strcmp(expr->expr.operator, "+") == 0) {
                generateTAC("ADD", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "-") == 0) {
                generateTAC("SUB", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "*") == 0) {
                generateTAC("MUL", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "/") == 0) {
                // Add division by zero check if needed
                generateTAC("DIV", tempVar, leftVar, rightVar);
            } else {
                fprintf(stderr, "Error: Unsupported operator '%s'.\n", expr->expr.operator);
                exit(EXIT_FAILURE);
            }
            return tempVar;
        }
        // if both left and right are integers, the result should be an integer
        else if (strcmp(determineExprType(expr->expr.left, symTab), "int") == 0 && strcmp(determineExprType(expr->expr.right, symTab), "int") == 0) {
            char* tempVar = generateTempIntVar();

            // Handle the different operators
            if (strcmp(expr->expr.operator, "+") == 0) {
                generateTAC("ADD", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "-") == 0) {
                generateTAC("SUB", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "*") == 0) {
                generateTAC("MUL", tempVar, leftVar, rightVar);
            } else if (strcmp(expr->expr.operator, "/") == 0) {
                // Add division by zero check if needed
                generateTAC("DIV", tempVar, leftVar, rightVar);
            } else {
                fprintf(stderr, "Error: Unsupported operator '%s'.\n", expr->expr.operator);
                exit(EXIT_FAILURE);
            }
            return tempVar;
        }
    return NULL; // Default return value if TAC generation fails
    } 
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
    static int tacWrittenToFile = 0;  // Static flag to track if the TAC has already been written to the file
    TAC* current = tacHead;
    FILE* tacFile = NULL;

    // Only open the file and write if it's the first time this function is called
    if (!tacWrittenToFile) {
        tacFile = fopen("TAC.ir", "w");  // Open the file for writing
        if (tacFile == NULL) {
            fprintf(stderr, "Error: Could not open TAC.ir for writing.\n");
            return;
        }
        tacWrittenToFile = 1;  // Set the flag to prevent further file writes
    }

    while (current != NULL) {
        // Ensure that each part of the TAC is checked for NULL to prevent crashes.
        const char* result = current->result ? current->result : "";
        const char* operand1 = current->operand1 ? current->operand1 : "";
        const char* operation = current->operation ? current->operation : "";
        const char* operand2 = current->operand2 ? current->operand2 : "";

        // Print format depending on operation type.
        if (strcmp(operation, "MOV") == 0) {
            printf("%s = %s %s\n", result, operation, operand1);
            if (tacFile) fprintf(tacFile, "%s = %s %s\n", result, operation, operand1);
        } else if (strlen(operand2) > 0) {
            printf("%s = %s %s %s\n", result, operand1, operation, operand2);
            if (tacFile) fprintf(tacFile, "%s = %s %s %s\n", result, operand1, operation, operand2);
        } else {
            printf("%s = %s %s\n", result, operation, operand1);
            if (tacFile) fprintf(tacFile, "%s = %s %s\n", result, operation, operand1);
        }

        current = current->next;
    }

    if (tacFile) {
        fclose(tacFile);  // Close the file after writing
    }
}


void printOptimizedTAC() {
    static int optimizedTACWrittenToFile = 0;  // Static flag to track if the optimized TAC has been written to the file
    TAC* current = tacHead; // Assuming tacHead points to the optimized TAC after optimization
    FILE* tacFile = NULL;

    // Only open the file and write if it's the first time this function is called
    if (!optimizedTACWrittenToFile) {
        tacFile = fopen("TACOptimized.ir", "w");  // Open the file for writing
        if (tacFile == NULL) {
            fprintf(stderr, "Error: Could not open TACOptimized.ir for writing.\n");
            return;
        }
        optimizedTACWrittenToFile = 1;  // Set the flag to prevent further file writes
    }

    while (current != NULL) {
        // Ensure that each part of the TAC is checked for NULL to prevent crashes.
        const char* result = current->result ? current->result : "";
        const char* operand1 = current->operand1 ? current->operand1 : "";
        const char* operation = current->operation ? current->operation : "";
        const char* operand2 = current->operand2 ? current->operand2 : "";

        // Print format depending on operation type.
        if (strcmp(operation, "MOV") == 0) {
            printf("%s = %s %s\n", result, operation, operand1);
            if (tacFile) fprintf(tacFile, "%s = %s %s\n", result, operation, operand1);
        } else if (strlen(operand2) > 0) {
            printf("%s = %s %s %s\n", result, operand1, operation, operand2);
            if (tacFile) fprintf(tacFile, "%s = %s %s %s\n", result, operand1, operation, operand2);
        } else {
            printf("%s = %s %s\n", result, operation, operand1);
            if (tacFile) fprintf(tacFile, "%s = %s %s\n", result, operation, operand1);
        }

        current = current->next;
    }

    if (tacFile) {
        fclose(tacFile);  // Close the file after writing
    }
}




char* determineExprType(ASTNode* expr, SymbolTable* symTab) {
    if (!expr) return NULL;

    switch (expr->type) {
        case NodeType_IntExpr:
            return "int";  // Assuming the expression is an integer
        case NodeType_FloatExpr:
            return "float";  // Assuming the expression is an integer
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
            semanticCheck(node->varDeclList.varDecl, symTab);
            semanticCheck(node->varDeclList.varDeclList, symTab);
            break;

        case NodeType_VarDecl:
            // Variable declaration; ensure the variable is added to the symbol table elsewhere
            printf("Adding variable '%s' of type '%s' to the symbol table.\n", node->varDecl.varName, node->varDecl.varType);
            break;

        case NodeType_IntExpr:
            printf("Integer expression with value: %d\n", node->IntExpr.integer);
            break;

        case NodeType_FloatExpr:
            printf("Float expression with value: %f\n", node->FloatExpr.floatNum);
            break;

        case NodeType_SimpleID: {
            // Ensure variable is declared
            printf("Looking up variable '%s'...\n", node->simpleID.name);
            Symbol* sym = lookupSymbol(symTab, node->simpleID.name);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->simpleID.name);
                exit(EXIT_FAILURE);
            }
            break;
        }

        case NodeType_Expr:
            printf("Checking expression with operator '%s'...\n", node->expr.operator);

            // Recursively check left and right sides
            semanticCheck(node->expr.left, symTab);
            semanticCheck(node->expr.right, symTab);

            // Type checking for binary operations
            char* leftType = determineExprType(node->expr.left, symTab);
            char* rightType = determineExprType(node->expr.right, symTab);
            if (strcmp(leftType, rightType) != 0) {
                fprintf(stderr, "Semantic Error: Type mismatch in expression with operator '%s'. Left type: %s, Right type: %s.\n",
                        node->expr.operator, leftType, rightType);
                exit(EXIT_FAILURE);
            }
            break;

        case NodeType_StmtList:
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
            break;
        }

        case NodeType_AssignStmt: {
            printf("Checking assignment to variable '%s'...\n", node->assignStmt.varName);
            Symbol* sym = lookupSymbol(symTab, node->assignStmt.varName);
            if (!sym) {
                fprintf(stderr, "Semantic Error: Variable '%s' used before declaration.\n", node->assignStmt.varName);
                exit(EXIT_FAILURE);
            }

            // Check if assignment type matches variable type
            char* exprType = determineExprType(node->assignStmt.expr, symTab);
            if (strcmp(sym->type, exprType) != 0) {
                fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'. Expected: %s, Found: %s.\n",
                        node->assignStmt.varName, sym->type, exprType);
                exit(EXIT_FAILURE);
            }

            // Check the expression being assigned
            semanticCheck(node->assignStmt.expr, symTab);
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