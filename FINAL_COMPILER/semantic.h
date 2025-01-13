#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "AST.h"

// Structure for TAC representation
typedef struct TAC {
    char* operator;      // The operation (e.g., +, -, *, /, =)
    char* arg1;          // First argument
    char* arg2;          // Second argument (if any)
    char* result;        // Result or destination
    struct TAC* next;    // Pointer to the next TAC
} TAC;

/* 
 * Enum to represent the data type of an AST node: int or float.
 */
typedef enum {
    DataType_Int,
    DataType_Float
} DataType;

// Function prototypes
TAC* createTAC(char* operator, char* arg1, char* arg2, char* result);
TAC* appendTAC(TAC* head, TAC* newTAC);
TAC* generateTAC(ASTNode* node, char* target);
void printTAC(TAC* tac);
void freeTAC(TAC* tac);

#endif // SEMANTIC_H
