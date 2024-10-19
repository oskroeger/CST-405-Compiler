#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "semantic.h"
#include "optimizer.h"
#include <stdbool.h>


// Function to generate MIPS code and write it to an output file
void generateMIPS(TAC* tacHead) {
    FILE* file = fopen("output.s", "w");
    if (!file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    // Write headers to the file
    fprintf(file, ".data\n");
    fprintf(file, "newline: .asciiz \"\\n\"\n");
    fprintf(file, ".text\n");
    fprintf(file, ".globl main\n");
    fprintf(file, "main:\n");

    TAC* current = tacHead;

    // Write MIPS instructions based on TAC
    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0) {
            if (isConstant(current->operand1)) {
                fprintf(file, "    li $%s, %s\n", current->result, current->operand1); // Load immediate value into the register
            } else {
                fprintf(file, "    move $%s, $%s\n", current->result, current->operand1); // Move value between registers
            }
        } else if (strcmp(current->operation, "ADD") == 0) {
            fprintf(file, "    add $%s, $%s, $%s\n", current->result, current->operand1, current->operand2); // Add operation between registers
        } else if (strcmp(current->operation, "WRITE") == 0) {
            // MIPS syscall to print integer
            fprintf(file, "    move $a0, $%s\n", current->result); // Move value from the specified register to $a0 for printing
            fprintf(file, "    li $v0, 1\n"); // Load syscall code for print integer
            fprintf(file, "    syscall\n"); // Execute syscall
            fprintf(file, "    li $v0, 4\n"); // Load syscall code for print string
            fprintf(file, "    la $a0, newline\n"); // Load the address of the newline string into $a0
            fprintf(file, "    syscall\n"); // Execute syscall
        }
        current = current->next;
    }

    // Write exit syscall
    fprintf(file, "    li $v0, 10\n");
    fprintf(file, "    syscall");

    fclose(file);
    printf("MIPS code written to output.s\n");
}