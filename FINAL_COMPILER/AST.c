#include "AST.h"
#include <float.h>
#include <string.h>

// Set the indentation level
int indentValue = 2;

// Print indentation for the current level
void printIndent(int level) {
    printf("%*s", level * indentValue, "");
}

// Map NodeType enums to readable strings
const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType_Program: return "Program";
        case NodeType_FunctionDefList: return "Function Definitions";
        case NodeType_FunctionDef: return "Function Definition";
        case NodeType_FunctionCall: return "Function Call";
        case NodeType_VarDeclList: return "Variable Declarations";
        case NodeType_VarDecl: return "Variable Declaration";
        case NodeType_IntExpr: return "Integer Expression";
        case NodeType_FloatExpr: return "Float Expression";
        case NodeType_SimpleID: return "Identifier";
        case NodeType_WriteStmt: return "Write Statement";
        case NodeType_Expr: return "Expression";
        case NodeType_StmtList: return "Statements";
        case NodeType_AssignStmt: return "Assignment Statement";
        case NodeType_ArrayDecl: return "Array Declaration";
        case NodeType_ArrayAccess: return "Array Access";
        case NodeType_IfStmt: return "If Statement";
        case NodeType_ReturnStmt: return "Return Statement";
        case NodeType_WhileStmt: return "While Loop";
        default: return "Unknown";
    }
}

// Traverse and print the AST
void traverseAST(ASTNode* node, int level) {
    if (!node) return;

    printIndent(level);
    printf("|-- %s", nodeTypeToString(node->type));

    switch (node->type) {
        case NodeType_Program:
            printf("\n");
            traverseAST(node->program.functionDefList, level + 1);
            traverseAST(node->program.varDeclList, level + 1);
            traverseAST(node->program.stmtList, level + 1);
            break;
        case NodeType_FunctionDefList:
            printf("\n");
            traverseAST(node->functionDefList.functionDef, level + 1);
            traverseAST(node->functionDefList.functionDefList, level + 1);
            break;
        case NodeType_FunctionDef:
            printf(": %s  // Function Definition\n", node->functionDef.name);
            traverseAST(node->functionDef.varDeclList, level + 1);
            traverseAST(node->functionDef.stmtList, level + 1);
            break;
        case NodeType_FunctionCall:
            printf(": %s()  // Function Call\n", node->functionCall.name);
            if (node->functionCall.args) {
                printIndent(level + 1);
                printf("|-- Arguments:\n");
                // node->functionCall.args is a StmtList representing arguments as expressions
                traverseAST(node->functionCall.args, level + 2);
            }
            break;
        case NodeType_VarDeclList:
            printf("\n");
            traverseAST(node->varDeclList.varDecl, level + 1);
            traverseAST(node->varDeclList.varDeclList, level + 1);
            break;
        case NodeType_VarDecl:
            printf(": %s %s\n", node->varDecl.varType, node->varDecl.varName);
            break;
        case NodeType_IntExpr:
            printf(": %d\n", node->IntExpr.integer);
            break;
        case NodeType_FloatExpr:
            printf(": %f\n", node->FloatExpr.floatNum);
            break;
        case NodeType_SimpleID:
            printf(": %s\n", node->simpleID.name);
            break;
        case NodeType_WriteStmt:
            printf(": write %s\n", node->writeStmt.id);
            break;
        case NodeType_Expr:
            printf(": Operator '%s'\n", node->expr.operator);
            traverseAST(node->expr.left, level + 1);
            traverseAST(node->expr.right, level + 1);
            break;
        case NodeType_StmtList:
            printf("\n");
            traverseAST(node->stmtList.stmt, level + 1);
            traverseAST(node->stmtList.stmtList, level + 1);
            break;
        case NodeType_AssignStmt:
            printf(": %s = \n", node->assignStmt.varName);
            traverseAST(node->assignStmt.expr, level + 1);
            break;
        case NodeType_ArrayDecl:
            printf(": %s %s[%d]  // Array Declaration\n", node->arrayDecl.varType, node->arrayDecl.varName, node->arrayDecl.size);
            break;
        case NodeType_ArrayAccess:
            printf(": %s[...]\n", node->arrayAccess.arrayName);
            traverseAST(node->arrayAccess.index, level + 1); // Traverse the index expression
            break;
        case NodeType_IfStmt:
            printf("\n");
            printIndent(level + 1);
            printf("|-- Condition:\n");
            traverseAST(node->ifStmt.condition, level + 2);
            printIndent(level + 1);
            printf("|-- Then:\n");
            traverseAST(node->ifStmt.thenStmt, level + 2);
            if (node->ifStmt.elseStmt) {
                printIndent(level + 1);
                printf("|-- Else:\n");
                traverseAST(node->ifStmt.elseStmt, level + 2);
            }
            break;
        case NodeType_ReturnStmt:
            printf("\n");
            printIndent(level + 1);
            printf("|-- return expr:\n");
            traverseAST(node->returnStmt.expr, level + 2);
            break;
        case NodeType_WhileStmt:
            printf(" \n");
            printIndent(level + 1);
            printf("Condition:\n");
            traverseAST(node->whileStmt.condition, level + 2);
            printIndent(level + 1);
            printf("Body:\n");
            traverseAST(node->whileStmt.body, level + 2);
            break;
        default:
            printf(" (Unknown node type)\n");
            break;
    }
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NodeType_Program:
            freeAST(node->program.functionDefList);
            freeAST(node->program.varDeclList);
            freeAST(node->program.stmtList);
            break;
        case NodeType_FunctionDefList:
            freeAST(node->functionDefList.functionDef);
            freeAST(node->functionDefList.functionDefList);
            break;
        case NodeType_FunctionDef:
            free(node->functionDef.name);
            freeAST(node->functionDef.varDeclList);
            freeAST(node->functionDef.stmtList);
            break;
        case NodeType_FunctionCall:
            free(node->functionCall.name);
            freeAST(node->functionCall.args); // Free argument list
            break;
        case NodeType_VarDeclList:
            freeAST(node->varDeclList.varDecl);
            freeAST(node->varDeclList.varDeclList);
            break;
        case NodeType_VarDecl:
            free(node->varDecl.varType);
            free(node->varDecl.varName);
            break;
        case NodeType_IntExpr:
            // No dynamic memory to free in IntExpr
            break;
        case NodeType_FloatExpr:
            // No dynamic memory to free in FloatExpr
            break;
        case NodeType_WriteStmt:
            free(node->writeStmt.id);
            break;
        case NodeType_SimpleID:
            free(node->simpleID.name);
            break;
        case NodeType_Expr:
            free(node->expr.operator);
            freeAST(node->expr.left);
            freeAST(node->expr.right);
            break;
        case NodeType_StmtList:
            freeAST(node->stmtList.stmt);
            freeAST(node->stmtList.stmtList);
            break;
        case NodeType_AssignStmt:
            free(node->assignStmt.varName);
            free(node->assignStmt.operator);
            freeAST(node->assignStmt.expr);
            break;
        case NodeType_ArrayDecl:
            free(node->arrayDecl.varType);
            free(node->arrayDecl.varName);
            break;
        case NodeType_ArrayAccess:
            free(node->arrayAccess.arrayName);
            freeAST(node->arrayAccess.index);
            break;
        case NodeType_IfStmt:
            freeAST(node->ifStmt.condition);
            freeAST(node->ifStmt.thenStmt);
            if (node->ifStmt.elseStmt) {
                freeAST(node->ifStmt.elseStmt);
            }
            break;
        case NodeType_ReturnStmt:
            freeAST(node->returnStmt.expr);
            break;
        case NodeType_WhileStmt:
            freeAST(node->whileStmt.condition);
            freeAST(node->whileStmt.body);
            break;
    }

    free(node);
}

ASTNode* createNode(NodeType type) {
    ASTNode* newNode = (ASTNode*)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Error: Memory allocation for new ASTNode failed.\n");
        return NULL;
    }

    newNode->type = type;

    // Initialize the node based on its type
    switch (type) {
        case NodeType_Program:
            newNode->program.functionDefList = NULL;
            newNode->program.varDeclList = NULL;
            newNode->program.stmtList = NULL;
            break;
        case NodeType_FunctionDefList:
            newNode->functionDefList.functionDef = NULL;
            newNode->functionDefList.functionDefList = NULL;
            break;
        case NodeType_FunctionDef:
            newNode->functionDef.name = NULL;
            newNode->functionDef.varDeclList = NULL;
            newNode->functionDef.stmtList = NULL;
            break;
        case NodeType_FunctionCall:
            newNode->functionCall.name = NULL;
            newNode->functionCall.args = NULL; // Initialize args to NULL
            break;
        case NodeType_VarDeclList:
            newNode->varDeclList.varDecl = NULL;
            newNode->varDeclList.varDeclList = NULL;
            break;
        case NodeType_VarDecl:
            newNode->varDecl.varType = NULL;
            newNode->varDecl.varName = NULL;
            break;
        case NodeType_IntExpr:
            newNode->IntExpr.integer = 0;
            break;
        case NodeType_FloatExpr:
            newNode->FloatExpr.floatNum = 0.0;
            break;
        case NodeType_SimpleID:
            newNode->simpleID.name = NULL;
            break;
        case NodeType_WriteStmt:
            newNode->writeStmt.id = NULL;
            break;
        case NodeType_Expr:
            newNode->expr.operator = NULL;
            newNode->expr.left = NULL;
            newNode->expr.right = NULL;
            break;
        case NodeType_StmtList:
            newNode->stmtList.stmt = NULL;
            newNode->stmtList.stmtList = NULL;
            break;
        case NodeType_AssignStmt:
            newNode->assignStmt.operator = NULL;
            newNode->assignStmt.varName = NULL;
            newNode->assignStmt.expr = NULL;
            break;
        case NodeType_ArrayDecl:
            newNode->arrayDecl.varType = NULL;
            newNode->arrayDecl.varName = NULL;
            newNode->arrayDecl.size = 0;
            break;
        case NodeType_ArrayAccess:
            newNode->arrayAccess.arrayName = NULL;
            newNode->arrayAccess.index = NULL;
            break;
        case NodeType_IfStmt:
            newNode->ifStmt.condition = NULL;
            newNode->ifStmt.thenStmt = NULL;
            newNode->ifStmt.elseStmt = NULL;
            break;
        case NodeType_ReturnStmt:
            newNode->returnStmt.expr = NULL;
            break;
        case NodeType_WhileStmt:
            newNode->whileStmt.condition = NULL;
            newNode->whileStmt.body = NULL;
            break;
    }

    return newNode;
}

float evaluateExpr(ASTNode* expr, SymbolTable* symTab) {
    if (expr->type == NodeType_IntExpr) {
        return (float) expr->IntExpr.integer;
    } else if (expr->type == NodeType_FloatExpr) {
        return expr->FloatExpr.floatNum;
    } else if (expr->type == NodeType_SimpleID) {
        // Handle simple variable access (int or float)
        Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
        if (sym != NULL) {
            if (sym->type == TYPE_INT) {
                if (sym->isParameter || sym->value.intValue != INT_MIN) {
                    if (sym->isParameter) {
                        // Parameters do not have a known value at compile time
                        // We can handle this by generating code or using a placeholder
                        fprintf(stderr, "Warning: Parameter '%s' used in expression cannot be evaluated at compile time.\n", sym->name);
                        // For simplicity, return 0.0
                        return 0.0f;
                    } else {
                        printf("[DEBUG] Simple ID '%s' evaluated to %d\n", sym->name, sym->value.intValue);
                        return (float) sym->value.intValue;
                    }
                } else {
                    fprintf(stderr, "Error: Variable '%s' used before being defined.\n", expr->simpleID.name);
                    exit(EXIT_FAILURE);
                }
            } else if (sym->type == TYPE_FLOAT) {
                if (sym->isParameter || sym->value.floatValue != FLT_MIN) {
                    if (sym->isParameter) {
                        // Parameters do not have a known value at compile time
                        fprintf(stderr, "Warning: Parameter '%s' used in expression cannot be evaluated at compile time.\n", sym->name);
                        // For simplicity, return 0.0
                        return 0.0f;
                    } else {
                        printf("[DEBUG] Simple ID '%s' evaluated to %f\n", sym->name, sym->value.floatValue);
                        return sym->value.floatValue;
                    }
                } else {
                    fprintf(stderr, "Error: Variable '%s' used before being defined.\n", expr->simpleID.name);
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            fprintf(stderr, "Error: Variable '%s' used before declaration.\n", expr->simpleID.name);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_ArrayAccess) {
        // Handle array access (int or float arrays)
        Symbol* sym = lookupSymbol(symTab, expr->arrayAccess.arrayName);
        if (sym != NULL) {
            int index = (int)evaluateExpr(expr->arrayAccess.index, symTab);  // Recursively evaluate index expression
            if (index >= 0 && index < sym->size) {
                // Check for int or float array types
                if (sym->type == TYPE_INT) {
                    printf("[DEBUG] Array '%s' index %d evaluated to %d\n", sym->name, index, sym->value.intArray[index]);
                    return (float)sym->value.intArray[index]; // Return the value of the int array element
                } else if (sym->type == TYPE_FLOAT) {
                    printf("[DEBUG] Array '%s' index %d evaluated to %f\n", sym->name, index, sym->value.floatArray[index]);
                    return sym->value.floatArray[index]; // Return the value of the float array element
                }
            } else {
                fprintf(stderr, "Error: Array index %d out of bounds for array '%s'.\n", index, expr->arrayAccess.arrayName);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Error: Array '%s' used before declaration.\n", expr->arrayAccess.arrayName);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_Expr) {
        // Handle binary expressions (+, -, *, /)
        float leftVal = evaluateExpr(expr->expr.left, symTab);  // Recursively evaluate left expression
        float rightVal = evaluateExpr(expr->expr.right, symTab); // Recursively evaluate right expression

        // Apply the operator to the evaluated left and right values
        if (strcmp(expr->expr.operator, "+") == 0) {
            printf("[DEBUG] Expression evaluated: %f + %f\n", leftVal, rightVal);
            return leftVal + rightVal;
        } else if (strcmp(expr->expr.operator, "-") == 0) {
            printf("[DEBUG] Expression evaluated: %f - %f\n", leftVal, rightVal);
            return leftVal - rightVal;
        } else if (strcmp(expr->expr.operator, "*") == 0) {
            printf("[DEBUG] Expression evaluated: %f * %f\n", leftVal, rightVal);
            return leftVal * rightVal;
        } else if (strcmp(expr->expr.operator, "/") == 0) {
            if (rightVal == 0) {
                printf("[WARNING] Dividing by zero.");
            }
            printf("[DEBUG] Expression evaluated: %f / %f\n", leftVal, rightVal);
            return leftVal / rightVal;
        } else if (strcmp(expr->expr.operator, "==") == 0) {
            printf("[DEBUG] Expression evaluated: %f == %f\n", leftVal, rightVal);
            return leftVal == rightVal;
        } else if (strcmp(expr->expr.operator, "!=") == 0) {
            printf("[DEBUG] Expression evaluated: %f != %f\n", leftVal, rightVal);
            return leftVal != rightVal;
        } else if (strcmp(expr->expr.operator, "<") == 0) {
            printf("[DEBUG] Expression evaluated: %f < %f\n", leftVal, rightVal);
            return leftVal < rightVal;
        } else if (strcmp(expr->expr.operator, "<=") == 0) {
            printf("[DEBUG] Expression evaluated: %f <= %f\n", leftVal, rightVal);
            return leftVal <= rightVal;
        } else if (strcmp(expr->expr.operator, ">") == 0) {
            printf("[DEBUG] Expression evaluated: %f > %f\n", leftVal, rightVal);
            return leftVal > rightVal;
        } else if (strcmp(expr->expr.operator, ">=") == 0) {
            printf("[DEBUG] Expression evaluated: %f >= %f\n", leftVal, rightVal);
            return leftVal >= rightVal;
        } else {
            fprintf(stderr, "Error: Unsupported operator '%s'.\n", expr->expr.operator);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_FunctionCall) {
        // For simplicity, function calls return 0.0 in expression evaluation
        printf("[DEBUG] Function call '%s()' in expression (returns 0.0)\n", expr->functionCall.name);
        return 0.0f;
    }

    // Return 0.0 if no valid expression type was matched (fail-safe)
    return 0.0f;
}