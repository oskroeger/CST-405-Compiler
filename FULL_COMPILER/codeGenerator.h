#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "semantic.h"
#include "symbolTable.h"
#include <stdio.h>

// Function to generate MIPS assembly code from TAC
void generateMIPSFromTAC(TAC* tacHead, const char* outputFilename);

#endif // CODE_GENERATOR_H
