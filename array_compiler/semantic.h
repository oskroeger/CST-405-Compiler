#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "AST.h"
#include "symbolTable.h"

// Define a structure for TAC instructions
typedef struct TAC {
    char* operation;     // The operation, e.g., "ADD", "SUB", "MUL", "DIV"
    char* result;        // The destination variable
    char* operand1;      // The first operand
    char* operand2;      // The second operand (could be NULL for unary operations)
    SymbolType type;     // Type of the operation (int, float)
    struct TAC* next;    // Pointer to the next TAC instruction
} TAC;

extern TAC* tacHead;  // Head of the TAC linked list
extern TAC* tacTail;  // Tail of the TAC linked list

// Function prototypes for TAC generation and manipulation
void generateTAC(const char* operation, const char* result, const char* operand1, const char* operand2);
void printTAC();  // Prints the generated TAC

// Temporary variable generation for different types
char* generateTempIntVar();   // Generate a temporary variable for int expressions
char* generateTempFloatVar(); // Generate a temporary variable for float expressions

// Expression handling and TAC generation
char* generateExprTAC(ASTNode* expr, SymbolTable* symTab);

// Semantic checks and AST traversal
void semanticCheck(ASTNode* node, SymbolTable* symTab);
void checkSemantics(ASTNode* root, SymbolTable* symTab);

// Determine expression type during semantic analysis
SymbolType determineExprType(ASTNode* expr, SymbolTable* symTab);

#endif // SEMANTIC_H
