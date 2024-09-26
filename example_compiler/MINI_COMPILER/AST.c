#include "AST.h"
int indentValue = 2;

void printIndent(int level) {
    for (int i = 0; i < level-1; i++) {
        printf("--");
    }
}


void traverseAST(ASTNode* node, int level) {
    if (!node) {
        printf ("Nothing to traverse\n");
        return;
    }

    printIndent(level);
    //printf("Traversing node of type %d\n", node->type);
    
    switch (node->type) {
    typedef struct ASTNode {
        int type; // Add the "type" field
        union {
            struct {
                struct ASTNode* varDeclList;
                struct ASTNode* stmtList;
            } program;
            struct {
                struct ASTNode* varDecl;
                struct ASTNode* varDeclList;
            } varDeclList;
            struct {
                char* varType;
                char* varName;
            } varDecl;
            struct {
                int number;
            } simpleExpr;
            struct {
                char* name;
            } simpleID;
            struct {
                char operator;
                struct ASTNode* left;
                struct ASTNode* right;
            } expr;
            struct {
                struct ASTNode* stmt;
                struct ASTNode* stmtList;
            } stmtList;
            struct {
                char* varName;
                char* operator;
                struct ASTNode* expr;
            } assignStmt;
            struct {
                char operator;
                struct ASTNode* left;
                struct ASTNode* right;
            } binOp;
        };
    } ASTNode;
        case NodeType_Program:
            printIndent(level);
            printf("Program\n");
            traverseAST(node->program.varDeclList, level + 1);
            traverseAST(node->program.stmtList, level + 1);
            break;
        case NodeType_VarDeclList:
            printIndent(level);
            //printf("VarDeclList\n");
            traverseAST(node->varDeclList.varDecl, level + 1);
            traverseAST(node->varDeclList.varDeclList, level + 1);
            break;
        case NodeType_VarDecl:
            printIndent(level);
            printf("VarDecl: %s %s\n", node->varDecl.varType, node->varDecl.varName);
            break;
        case NodeType_SimpleExpr:
            printIndent(level);
            //printf("SimpleExpr: %d\n", node->simpleExpr.number);
            printf("%d\n", node->simpleExpr.number);
            break;
        case NodeType_SimpleID:
            printIndent(level);
            //printf("SimpleID: %d\n", node->simpleID.name);
            printf("%s\n", node->simpleID.name);
            break;
        case NodeType_Expr:
            printIndent(level);
            printf("Expr: %c\n", node->expr.operator);
            traverseAST(node->expr.left, level + 1);
            traverseAST(node->expr.right, level + 1);
            break;
        case NodeType_StmtList:
            printIndent(level);
            //printf("StmtList\n");
            traverseAST(node->stmtList.stmt, level + 1);
            traverseAST(node->stmtList.stmtList, level + 1);
            break;
        case NodeType_AssignStmt:
            printIndent(level);
            printf("Stmt: %s = ", node->assignStmt.varName);
            traverseAST(node->assignStmt.expr, level + 1);
            break;
        case NodeType_BinOp:
            printIndent(level);
            printf("BinOp: %c\n", node->binOp.operator);
            traverseAST(node->binOp.left, level + 1);
            traverseAST(node->binOp.right, level + 1);
            break;
    }
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NodeType_Program:
            free(node->program.varDeclList);
            free(node->program.stmtList);
            break;
        case NodeType_VarDeclList:
            free(node->varDeclList.varDecl);
            free(node->varDeclList.varDeclList);
            break;
        case NodeType_VarDecl:
            free(node->varDecl.varType);
            free(node->varDecl.varName);
            break;
        
        case NodeType_SimpleExpr:
            //free((node->simpleExpr));
            break;
        case NodeType_SimpleID:
            //free((node->simpleID));
            break;
        case NodeType_Expr:
            free(node->expr.left);
            free(node->expr.right);
            //free(node->expr.operator);
            break;
        case NodeType_StmtList:
            free(node->stmtList.stmt);
            free(node->stmtList.stmtList);
            break;
        case NodeType_AssignStmt:
            free(node->assignStmt.varName);
            free(node->assignStmt.operator);
            free(node->assignStmt.expr);
            break;
        case NodeType_BinOp:
            free(node->binOp.left);
            free(node->binOp.right);
            //free(node->binOp.operator);
            break;
    }

    free(node);
}

ASTNode* createNode(NodeType type) {
    ASTNode* newNode = (ASTNode*)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        // Handle memory allocation failure if needed
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
        case NodeType_SimpleExpr:
            // Initialize the number to NULL
            newNode->simpleExpr.number = '\0';
            break;
        case NodeType_SimpleID:
            //newNode->simpleID.name = NULL;
            break;
        case NodeType_Expr:
            newNode->expr.operator = '\0';  // Placeholder value
            newNode->expr.left = NULL;
            newNode->expr.right = NULL;
            break;
        case NodeType_StmtList:
            newNode->stmtList.stmt = NULL;  // Example initialization
            newNode->stmtList.stmtList = NULL;
            break;
        case NodeType_AssignStmt:
            newNode->assignStmt.operator = NULL;  // Example initialization
            newNode->assignStmt.varName = NULL;
            newNode->assignStmt.expr = NULL;
            break;
        case NodeType_BinOp:
            newNode->binOp.operator = '\0';  // Placeholder value
            newNode->binOp.left = NULL;
            newNode->binOp.right = NULL;
            break;
        // Add more cases as necessary for other node types
    }

    return newNode;
}