#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "semantic.h"
#include "optimizer.h"
#include <stdbool.h>


void generateMIPS(TAC* tacHead) {
    TAC* current = tacHead;
    printf("\n----- GENERATED MIPS CODE -----\n");

    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0) {
            if (isConstant(current->operand1)) {
                printf("li $%s, %s\n", current->result, current->operand1); // Load immediate value into the register
            } else {
                printf("move $%s, $%s\n", current->result, current->operand1); // Move value between registers
            }
        } else if (strcmp(current->operation, "ADD") == 0) {
            printf("add $%s, $%s, $%s\n", current->result, current->operand1, current->operand2); // Add operation between registers
        } else if (strcmp(current->operation, "WRITE") == 0) {
            // MIPS syscall to print integer
            printf("move $a0, $%s\n", current->result); // Move value from the specified register to $a0 for printing
            printf("li $v0, 1\n"); // Load syscall code for print integer
            printf("syscall\n"); // Execute syscall
        }
        current = current->next;
    }
}