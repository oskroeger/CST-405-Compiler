#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include "symbolTable.h"

// NodeType enum to differentiate between different kinds of AST nodes
typedef enum { 
    NodeType_Program,
    NodeType_FunctionDefList,  // For function definitions list
    NodeType_FunctionDef,      // For function definition
    NodeType_FunctionCall,     // For function calls
    NodeType_Param,            // For individual function parameters
    NodeType_ParamList,        // For a list of function parameters
    NodeType_ArgList,
    NodeType_ReturnStmt,       // For return statements
    NodeType_VarDeclList,
    NodeType_VarDecl,
    NodeType_IfStmt, 
    NodeType_IntExpr,
    NodeType_FloatExpr,
    NodeType_SimpleID,
    NodeType_Expr, 
    NodeType_StmtList,
    NodeType_WriteStmt,
    NodeType_AssignStmt,
    NodeType_ArrayDecl,        // Node type for array declarations
    NodeType_ArrayAccess       // Node type for array access
} NodeType;

// Structure for AST nodes
typedef struct ASTNode {
    NodeType type;
    union {
        struct {
            struct ASTNode* functionDefList; // List of function definitions
            struct ASTNode* varDeclList;    // Global variable declarations
            struct ASTNode* stmtList;       // Statements at the global level
        } program;

        struct {
            struct ASTNode* functionDef;
            struct ASTNode* functionDefList;
        } functionDefList;

        struct {
            char* name;                     // Name of the function
            struct ASTNode* paramList;      // List of parameters
            struct ASTNode* varDeclList;    // Local variable declarations
            struct ASTNode* stmtList;       // Statements inside the function
        } functionDef;

        struct {
            char* name;                     // Name of the function being called
            struct ASTNode* argList;        // List of arguments passed to the function
        } functionCall;

        struct {
            struct ASTNode* expr;       // The argument expression
            struct ASTNode* next;       // Next argument in the list
        } argList; 

        struct {
            struct ASTNode* param;          // Individual parameter
            struct ASTNode* next;           // Next parameter in the list
        } paramList;

        struct {
            char* type;                     // Type of the parameter (int, float, etc.)
            char* name;                     // Name of the parameter
        } param;

        struct {
            struct ASTNode* expr;           // Expression being returned
        } returnStmt;

        struct {
            struct ASTNode* varDecl;
            struct ASTNode* varDeclList;
        } varDeclList;

        struct {
            char* varType;
            char* varName;
        } varDecl;

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
            char* id;                       // The identifier to write
        } writeStmt;

        struct {
            char* varName;
            char* operator;
            struct ASTNode* expr;
        } assignStmt;

        // Structures for arrays
        struct {
            char* varType;                  // Type of array (int, float)
            char* varName;                  // Name of the array
            int size;                       // Size of the array
        } arrayDecl;

        struct {
            char* arrayName;                // The array being accessed
            struct ASTNode* index;          // The index expression
        } arrayAccess;
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
