#include "semantic.h"
#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void removeUnusedTemporaries(TAC** head) {
    TAC* current = *head;
    TAC* prev = NULL;

    while (current != NULL) {
        TAC* next = current->next;

        // Only remove MOV operations with temp variables that are no longer used
        if (strcmp(current->operation, "MOV") == 0 && current->result[0] == 't') {
            int isUsed = 0;
            TAC* check = *head;
            while (check != NULL) {
                if ((check->operand1 && strcmp(check->operand1, current->result) == 0) ||
                    (check->operand2 && strcmp(check->operand2, current->result) == 0)) {
                    isUsed = 1;
                    break;
                }
                check = check->next;
            }

            if (!isUsed) {
                // Remove the current TAC from the list
                if (prev == NULL) {
                    *head = current->next; // Update head if we're removing the first element
                } else {
                    prev->next = current->next; // Link previous node to the next one
                }
                free(current->result);
                free(current->operand1);
                free(current->operand2);
                free(current);
                current = next;
                continue;
            }
        }
        prev = current;
        current = next;
    }
}

void optimizeTAC() {
    TAC* current = tacHead;
    while (current != NULL) {
        constantFolding(current);
        copyPropagation(current);
        current = current->next;
    }

    // Remove unused temporary variables after propagation
    removeUnusedTemporaries(&tacHead);

    // Final pass to remove redundant MOVs and update final variables correctly
    eliminateRedundantAssignments(&tacHead);
}

void constantFolding(TAC* tac) {
    if (strcmp(tac->operation, "ADD") == 0 && isConstant(tac->operand1) && isConstant(tac->operand2)) {
        int val1 = atoi(tac->operand1);
        int val2 = atoi(tac->operand2);
        int result = val1 + val2;

        // Convert the result to a string
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%d", result);

        // Debugging Output
        printf("Constant Folding: %d + %d -> %s\n", val1, val2, buffer);

        // Update the TAC to reflect the constant folding
        if (tac->operand1) free(tac->operand1);
        tac->operand1 = strdup(buffer);
        if (!tac->operand1) {
            fprintf(stderr, "Memory allocation failed for operand1\n");
            exit(EXIT_FAILURE);
        }

        if (tac->operand2) {
            free(tac->operand2);
            tac->operand2 = NULL;  // Set to NULL as it's no longer needed
        }

        strcpy(tac->operation, "MOV");  // Change the operation to MOV

        // Debugging Output After Modification
        printf("After Folding: operation = %s, operand1 = %s, operand2 = %s\n", tac->operation, tac->operand1, tac->operand2);
    }
}

void copyPropagation(TAC* tac) {
    TAC* current = tacHead;
    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0) {
            // Replace usage of the result of MOV with the operand1 in subsequent TACs
            if (tac->operand1 && strcmp(current->result, tac->operand1) == 0) {
                free(tac->operand1);
                tac->operand1 = strdup(current->operand1);
            }

            if (tac->operand2 && strcmp(current->result, tac->operand2) == 0) {
                free(tac->operand2);
                tac->operand2 = strdup(current->operand1);
            }
        }
        current = current->next;
    }
}

void eliminateRedundantAssignments(TAC** head) {
    TAC* current = *head;
    TAC* prev = NULL;

    while (current != NULL) {
        TAC* next = current->next;

        if (strcmp(current->operation, "MOV") == 0) {
            TAC* check = next;
            int isRedundant = 0;

            // Check if there is another assignment to the same variable later
            while (check != NULL) {
                if (strcmp(check->operation, "MOV") == 0 && strcmp(check->result, current->result) == 0) {
                    isRedundant = 1;
                    break;
                }
                check = check->next;
            }

            if (isRedundant) {
                // Remove the current TAC from the list
                if (prev == NULL) {
                    *head = next; // Update head if we're removing the first element
                } else {
                    prev->next = next; // Link previous node to the next one
                }
                free(current->result);
                free(current->operand1);
                free(current->operand2);
                free(current);
                current = next;
                continue;
            }
        }

        prev = current;
        current = next;
    }
}

int isConstant(const char* operand) {
    if (operand == NULL) return 0;
    for (int i = 0; operand[i] != '\0'; i++) {
        if (!isdigit(operand[i])) return 0;
    }
    return 1;
}
