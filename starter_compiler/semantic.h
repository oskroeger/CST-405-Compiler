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
    struct TAC* next;    // Pointer to the next TAC instruction
} TAC;

extern TAC* tacHead;  // Head of the TAC linked list
extern TAC* tacTail;  // Tail of the TAC linked list

void generateTAC(const char* operation, const char* result, const char* operand1, const char* operand2);
void printTAC();
char* generateTempVar();
char* generateExprTAC(ASTNode* expr, SymbolTable* symTab);
void semanticCheck(ASTNode* node, SymbolTable* symTab);
void checkSemantics(ASTNode* root, SymbolTable* symTab);
char* determineExprType(ASTNode* expr, SymbolTable* symTab);

#endif // SEMANTIC_H