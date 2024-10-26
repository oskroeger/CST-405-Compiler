#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include "symbolTable.h"

// NodeType enum to differentiate between different kinds of AST nodes
typedef enum { 
    NodeType_Program,
    NodeType_VarDeclList, 
    NodeType_VarDecl, 
    NodeType_IntExpr,
    NodeType_FloatExpr,
    NodeType_SimpleID,
    NodeType_Expr, 
    NodeType_StmtList,
    NodeType_WriteStmt,
    NodeType_AssignStmt,
    NodeType_FuncDecl,
    NodeType_ParamList,
    NodeType_FuncDeclList,
    NodeType_ArrayDecl,    // Node type for array declarations
    NodeType_ArrayAccess   // Node type for array access
} NodeType;

// Structure for AST nodes
typedef struct ASTNode {
    NodeType type;
    union {
        struct {
            struct ASTNode* varDeclList;
            struct ASTNode* funcDeclList;
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
            struct ASTNode* funcDecl;
            struct ASTNode* funcDeclList;
        } funcDeclList;

        struct {
            char* funcType;
            char* funcName;
            //parameter list
            struct varDeclList* paramList;
            struct varDeclList* varDeclList;
            struct stmtList* stmtList;
        } funcDecl;

        struct {
            int integer;
        } IntExpr;

        struct {
            float floatNum;
        } FloatExpr;

        struct {
            char* name;
        } simpleID;

        struct {
            char* operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } expr;

        struct {
            struct ASTNode* stmt;
            struct ASTNode* stmtList;
        } stmtList;

        struct {
            char* id;  // The identifier to write
        } writeStmt;


        struct {
            char* varName;
            char* operator;
            struct ASTNode* expr;
        } assignStmt;

        // Structures for arrays
        struct {
            char* varType;  // Type of array (int, float)
            char* varName;  // Name of the array
            int size;       // Size of the array
        } arrayDecl;  // For array declarations

        struct {
            char* arrayName;      // The array being accessed
            struct ASTNode* index;  // The index expression
        } arrayAccess;  // For array access

        struct {
            struct ASTNode* param;
            struct ASTNode* paramList;
        } paramList;

        struct {
            struct ASTNode* funcDecl;
            struct ASTNode* funcDecList;
        } funcDecList;
        
    };
} ASTNode;

// Function prototypes for AST handling
ASTNode* createNode(NodeType type);
const char* nodeTypeToString(NodeType type);
void printIndent(int level);
void freeAST(ASTNode* node);
void traverseAST(ASTNode* node, int level);
float evaluateExpr(ASTNode* expr, SymbolTable* symTab);

#endif // AST_H
