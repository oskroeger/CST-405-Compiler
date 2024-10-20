#include "AST.h"
#include <float.h>
#include <string.h>

int indentValue = 2;

void printIndent(int level) {
    for (int i = 0; i < level; i++) {
        printf("   ");
    }
}

const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType_Program: return "Program";
        case NodeType_VarDeclList: return "Variable Declarations";
        case NodeType_VarDecl: return "Variable Declaration";
        case NodeType_IntExpr: return "Integer Expression";
        case NodeType_FloatExpr: return "Float Expression";
        case NodeType_SimpleID: return "Identifier";
        case NodeType_WriteStmt: return "Write Statement";
        case NodeType_Expr: return "Expression";
        case NodeType_StmtList: return "Statements";
        case NodeType_AssignStmt: return "Assignment Statement";
        default: return "Unknown";
    }
}

void traverseAST(ASTNode* node, int level) {
    if (!node) {
        return;
    }

    printIndent(level);
    printf("|-- %s", nodeTypeToString(node->type));

    switch (node->type) {
        case NodeType_Program:
            printf("\n");
            traverseAST(node->program.varDeclList, level + 1);
            traverseAST(node->program.stmtList, level + 1);
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
        default:
            printf(" (Unknown node type)\n");
            break;
    }
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NodeType_Program:
            freeAST(node->program.varDeclList);
            freeAST(node->program.stmtList);
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
            freeAST(node->assignStmt.expr);
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
            newNode->program.varDeclList = NULL;
            newNode->program.stmtList = NULL;
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
            newNode->expr.operator = NULL; // Initialize operator to NULL
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
        // Add more cases as necessary for other node types
    }

    return newNode;
}

float evaluateExpr(ASTNode* expr, SymbolTable* symTab) {
    if (expr->type == NodeType_IntExpr) {
        return (float) expr->IntExpr.integer;
    } else if (expr->type == NodeType_FloatExpr) {
        return expr->FloatExpr.floatNum;
    } else if (expr->type == NodeType_SimpleID) {
        Symbol* sym = lookupSymbol(symTab, expr->simpleID.name);
        if (sym != NULL) {
            if (sym->type == TYPE_INT && sym->value.intValue != INT_MIN) {
                return (float) sym->value.intValue;
            } else if (sym->type == TYPE_FLOAT && sym->value.floatValue != FLT_MIN) {
                return sym->value.floatValue;
            } else {
                fprintf(stderr, "Error: Variable '%s' used before being defined.\n", expr->simpleID.name);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Error: Variable '%s' used before declaration.\n", expr->simpleID.name);
            exit(EXIT_FAILURE);
        }
    } else if (expr->type == NodeType_Expr) {
        float leftVal = evaluateExpr(expr->expr.left, symTab);
        float rightVal = evaluateExpr(expr->expr.right, symTab);

        if (strcmp(expr->expr.operator, "+") == 0) {
            return leftVal + rightVal;
        } else if (strcmp(expr->expr.operator, "-") == 0) {
            return leftVal - rightVal;
        } else if (strcmp(expr->expr.operator, "*") == 0) {
            return leftVal * rightVal;
        } else if (strcmp(expr->expr.operator, "/") == 0) {
            if (rightVal == 0) {
                fprintf(stderr, "Error: Division by zero.\n");
                exit(EXIT_FAILURE);
            }
            return leftVal / rightVal;
        } else {
            fprintf(stderr, "Error: Unsupported operator '%s'.\n", expr->expr.operator);
            exit(EXIT_FAILURE);
        }
    }
    return 0.0;
}
