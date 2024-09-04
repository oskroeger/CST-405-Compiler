#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int getTempRegister(const char* tempVar);
void generateMIPS(TAC* head);

#endif // CODE_GENERATOR_H