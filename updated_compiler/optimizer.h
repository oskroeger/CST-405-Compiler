#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

TAC* createTAC(const char* operation, const char* result, const char* operand1, const char* operand2);
void optimizeTAC(TAC** head);
void constantFolding(TAC** head, TAC** optimizedHead);
void copyPropagation(TAC** head, TAC** optimizedHead);
void deadCodeElimination(TAC** head, TAC** optimizedHead);
bool isBinaryOperation(const char* operation);
void renumberRegisters(TAC** head);
void replaceVariablesWithTemp(TAC** head);
bool isVariable(const char* str);
bool isTemp(const char* str);
bool isConstant(const char* str);
char* createTempVar();
void appendTAC(TAC** head, TAC* newInstruction);

#endif // OPTIMIZER_H