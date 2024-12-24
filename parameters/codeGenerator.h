#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "semantic.h"  // For TAC
#include "symbolTable.h"

/*
 * Generates MIPS assembly from the given TAC list and writes it to outputFilename.
 */
void generateMIPSFromTAC(TAC* tacHead, const ScopeNode* scopeListHead, const char* outputFilename);

#endif // CODE_GENERATOR_H
