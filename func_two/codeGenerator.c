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
        char firstChar = current->result[0];  // Get the first character of the result variable (t for int, f for float)

        // Handle MOV operation
        if (strcmp(current->operation, "MOV") == 0) {
            if (isConstant(current->operand1)) {
                if (firstChar == 't') {  // Integer temp var
                    fprintf(file, "    li $%s, %s\n", current->result, current->operand1); // Load immediate integer
                } else if (firstChar == 'f') {  // Float temp var
                    // Format the float to 4 decimal places
                    float floatVal = atof(current->operand1);
                    fprintf(file, "    li.s $f%s, %.4f\n", current->result + 1, floatVal); // Load immediate float
                }
            } else {
                if (firstChar == 't') {  // Integer temp var
                    fprintf(file, "    move $%s, $%s\n", current->result, current->operand1); // Move integer
                } else if (firstChar == 'f') {  // Float temp var
                    fprintf(file, "    mov.s $f%s, $f%s\n", current->result + 1, current->operand1 + 1); // Move float
                }
            }
        }
        // Handle arithmetic operations (ADD, SUB, MUL, DIV)
        else if (strcmp(current->operation, "ADD") == 0 || strcmp(current->operation, "SUB") == 0 ||
                 strcmp(current->operation, "MUL") == 0 || strcmp(current->operation, "DIV") == 0) {
            if (firstChar == 't') {  // Integer temp var
                fprintf(file, "    %s $%s, $%s, $%s\n", current->operation, current->result, current->operand1, current->operand2); // Integer arithmetic
            } else if (firstChar == 'f') {  // Float temp var
                fprintf(file, "    %s.s $f%s, $f%s, $f%s\n", current->operation, current->result + 1, current->operand1 + 1, current->operand2 + 1); // Float arithmetic
            }
        }
        // Handle WRITE operation
        else if (strcmp(current->operation, "WRITE") == 0) {
            if (firstChar == 't') {  // Integer write
                fprintf(file, "    move $a0, $%s\n", current->result); // Move integer to $a0 for printing
                fprintf(file, "    li $v0, 1\n"); // Syscall for print integer
            } else if (firstChar == 'f') {  // Float write
                fprintf(file, "    mov.s $f12, $f%s\n", current->result + 1); // Move float to $f12 for printing
                fprintf(file, "    li $v0, 2\n"); // Syscall for print float
            }
            fprintf(file, "    syscall\n");
            fprintf(file, "    li $v0, 4\n"); // Syscall to print newline
            fprintf(file, "    la $a0, newline\n");
            fprintf(file, "    syscall\n");
        }

        current = current->next;
    }

    // Write exit syscall
    fprintf(file, "    li $v0, 10\n");
    fprintf(file, "    syscall\n");

    fclose(file);
    printf("MIPS code written to output.s\n");
}