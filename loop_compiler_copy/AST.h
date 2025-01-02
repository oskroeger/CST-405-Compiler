#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include "symbolTable.h"

// NodeType enum to differentiate between different kinds of AST nodes
typedef enum { 
    NodeType_Program,
    NodeType_FunctionDefList,  // Added for function definitions list
    NodeType_FunctionDef,      // Added for function definition
    NodeType_FunctionCall,     // Added for function calls
    NodeType_VarDeclList, 
    NodeType_VarDecl,
    NodeType_ReturnStmt,
    NodeType_IfStmt, 
    NodeType_IntExpr,
    NodeType_FloatExpr,
    NodeType_SimpleID,
    NodeType_Expr, 
    NodeType_StmtList,
    NodeType_WriteStmt,
    NodeType_AssignStmt,
    NodeType_ArrayDecl,    // Node type for array declarations
    NodeType_ArrayAccess,   // Node type for array access
    NodeType_WhileStmt,
} NodeType;

typedef enum {
    TYPE_I = 0,
    TYPE_F = 1,
    TYPE_C = 2,
} DataType;


// Structure for AST nodes
typedef struct ASTNode {
    NodeType type;
    DataType dataType; 
    union {
        struct {
            struct ASTNode* functionDefList; // Added for function definitions
            struct ASTNode* varDeclList;
            struct ASTNode* stmtList;
        } program;

        struct {
            struct ASTNode* functionDef;
            struct ASTNode* functionDefList;
        } functionDefList;  // Added for function definitions list

        struct {
            char* name;
            struct ASTNode* varDeclList;
            struct ASTNode* stmtList;
        } functionDef;  // Added for function definition

        struct {
            char* name;
            struct ASTNode* args; // A list of expressions for arguments
        } functionCall;  // Added for function calls

        struct {
            struct ASTNode* condition;
            struct ASTNode* thenStmt;
            struct ASTNode* elseStmt; // may be NULL
        } ifStmt;

        struct {
            struct ASTNode* varDecl;
            struct ASTNode* varDeclList;
        } varDeclList;

        struct {
            char* varType;
            char* varName;
        } varDecl;

        struct {
            struct ASTNode* expr;
        } returnStmt;

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
            int isArray;  
            struct ASTNode* arrayIndex; // If isArray=1, store the index expression here
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
            struct ASTNode* condition;  // The condition of the while loop
            struct ASTNode* body;       // The body of the while loop
        } whileStmt;
    };
} ASTNode;

// Function prototypes for AST handling
void evaluateIfStmt(ASTNode* ifStmt, SymbolTable* symTab);
ASTNode* createNode(NodeType type);
const char* nodeTypeToString(NodeType type);
void printIndent(int level);
void freeAST(ASTNode* node);
void traverseAST(ASTNode* node, int level);
float evaluateExpr(ASTNode* expr, SymbolTable* symTab);

#endif // AST_H