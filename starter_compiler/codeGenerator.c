#include "codeGenerator.h"
#include "semantic.h"
#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to map TAC temp variables to MIPS registers
int getTempRegister(const char* tempVar) {
    // Assuming tempVar is in the form "tX" where X is a number
    return atoi(tempVar + 1);  // Extract the number part and return it as the register number
}

void generateMIPS(TAC* head) {
    TAC* current = head;
    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0) {
            if (isConstant(current->operand1)) {
                printf("li $t%d, %s\n", getTempRegister(current->result), current->operand1);
            } else {
                printf("move $t%d, $t%d\n", getTempRegister(current->result), getTempRegister(current->operand1));
            }
        } else if (strcmp(current->operation, "ADD") == 0) {
            printf("add $t%d, $t%d, $t%d\n",
                   getTempRegister(current->result),
                   getTempRegister(current->operand1),
                   getTempRegister(current->operand2));
        }
        // Add more cases for other operations like SUB, MUL, etc.
        current = current->next;
    }
}