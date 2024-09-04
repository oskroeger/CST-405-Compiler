#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void constantFolding(TAC* tac);
void copyPropagation(TAC* tac);
int isConstant(const char* operand);
void optimizeTAC();
void removeUnusedTemporaries(TAC** head);
void eliminateRedundantAssignments(TAC** head);

#endif // OPTIMIZER_H