#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "semantic.h" // For TAC
#include "symbolTable.h" // For SymbolType and symbol lookup

void generateMIPS(TAC* tacHead, const char* outputFilename);

#endif // CODE_GENERATOR_H
