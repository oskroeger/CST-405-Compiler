/*
Adding an optimization phase for Three-Address Code (TAC) before generating MIPS assembly 
is a crucial step to enhance the efficiency of your generated code. 
Optimizations can reduce the number of instructions, minimize memory access, 
and improve overall execution speed. Below, we'll outline a basic approach for 
implementing a TAC optimizer. This optimizer will focus on simple peephole optimizations,
which look at small sets of instructions (or "peepholes") to apply local optimizations.

Basic Optimizations:
-------------------
1. Constant Folding: Evaluate expressions involving constants at compile time instead of runtime.
2. Constant Propagation: Replace variables with known constant values.
3. Copy Propagation: Replace uses of a variable that has been assigned the value of another variable.
4. Dead Code Elimination: Remove instructions that compute values not used by subsequent instructions 
   or the program's output.
*/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "semantic.h"
#include <stdbool.h>
#include <ctype.h>

void optimizeTAC(TAC** head);
bool isConstant(const char* str);
bool isVariable(const char* str);
void constantFolding(TAC** head);
void constantPropagation(TAC** head);
void copyPropagation(TAC** head);
void deadCodeElimination(TAC** head);
void printOptimizedTAC(const char* filename, TAC* head);
void printCurrentOptimizedTAC(TAC* current);

#endif // OPTIMIZER_H