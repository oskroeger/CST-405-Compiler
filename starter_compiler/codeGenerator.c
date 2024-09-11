#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "optimizer.h"

#define MAX_REGISTERS 10

// Helper function to check if a result is used later in the TAC
int isUsedLater(TAC* current, const char* result) {
    while (current != NULL) {
        if ((current->operand1 && strcmp(current->operand1, result) == 0) ||
            (current->operand2 && strcmp(current->operand2, result) == 0)) {
            return 1; // Found a use of the result later in the TAC
        }
        current = current->next;
    }
    return 0; // No use found
}

// Mapping variables to registers
void generateMIPS(TAC* tacHead) {
    TAC* current = tacHead;
    char* registerMap[MAX_REGISTERS] = {NULL}; // Map variables to registers
    int regCounter = 0;

    printf("\n----- GENERATED MIPS CODE -----\n");

    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0) {
            // Generate code for MOV operation
            if (isConstant(current->operand1)) {
                // Directly load immediate value into register
                printf("li $t%d, %s\n", regCounter, current->operand1); 
            } else {
                // Check if the move is necessary
                int srcReg = -1;
                for (int i = 0; i < regCounter; i++) {
                    if (registerMap[i] && strcmp(registerMap[i], current->operand1) == 0) {
                        srcReg = i;
                        break;
                    }
                }
                if (srcReg != -1) {
                    // Only generate the move if the result is used later
                    if (isUsedLater(current->next, current->result)) {
                        printf("move $t%d, $t%d\n", regCounter, srcReg);
                    }
                } else {
                    printf("Error: Operand %s not found in any register.\n", current->operand1);
                }
            }
            registerMap[regCounter] = current->result; // Map result to the current register
            regCounter = (regCounter + 1) % MAX_REGISTERS; // Wrap regCounter if it exceeds max registers
        } else if (strcmp(current->operation, "ADD") == 0) {
            // Generate code for ADD operation
            int reg1 = -1, reg2 = -1;

            // Find registers for operand1 and operand2
            for (int i = 0; i < regCounter; i++) {
                if (registerMap[i] && strcmp(registerMap[i], current->operand1) == 0) reg1 = i;
                if (registerMap[i] && strcmp(registerMap[i], current->operand2) == 0) reg2 = i;
            }

            // Load immediate values if registers are not found
            if (reg1 == -1 && isConstant(current->operand1)) {
                printf("li $t%d, %s\n", regCounter, current->operand1);
                reg1 = regCounter++;
            }
            if (reg2 == -1 && isConstant(current->operand2)) {
                printf("li $t%d, %s\n", regCounter, current->operand2);
                reg2 = regCounter++;
            }

            printf("add $t%d, $t%d, $t%d\n", regCounter, reg1, reg2); // Add the operands
            registerMap[regCounter] = current->result;
            regCounter = (regCounter + 1) % MAX_REGISTERS; // Wrap regCounter if it exceeds max registers
        }

        current = current->next;
    }
}
