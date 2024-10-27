#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Update the createTAC function to include SymbolType for handling int and float types
TAC* createTAC(const char* operation, const char* result, const char* operand1, const char* operand2, SymbolType type);

// Main optimization function
void optimizeTAC(TAC** head);

// Specific optimization passes
void constantFolding(TAC** head, TAC** optimizedHead);
void copyPropagation(TAC** head, TAC** optimizedHead);
void deadCodeElimination(TAC** head, TAC** optimizedHead);

// Helper functions
bool isBinaryOperation(const char* operation);
void renumberRegisters(TAC** head);
void replaceVariablesWithTemp(TAC** head, SymbolTable* symTab);
bool isArrayAccess(const char* str);
bool isVariable(const char* str);
bool isTemp(const char* str);
bool isConstant(const char* str);
bool isFloatConstant(const char* str);

// Function to create temporary variables, now with a bool indicating whether it's for floats
char* createTempVar(bool isFloat);

// Function to safely append TAC instructions to the linked list
void appendTAC(TAC** head, TAC* newInstruction);

char* getArrayName(const char* arrayAccessStr);
void printCurrentTAC(TAC* currentHead);

#endif // OPTIMIZER_H
