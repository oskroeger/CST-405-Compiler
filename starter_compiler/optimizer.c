#include "optimizer.h"
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to create a new TAC node
TAC* createTAC(const char* operation, const char* result, const char* operand1, const char* operand2) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    if (!newTAC) {
        fprintf(stderr, "Memory allocation failed for new TAC node.\n");
        exit(EXIT_FAILURE);
    }
    newTAC->operation = strdup(operation);
    newTAC->result = result ? strdup(result) : NULL;
    newTAC->operand1 = operand1 ? strdup(operand1) : NULL;
    newTAC->operand2 = operand2 ? strdup(operand2) : NULL;
    newTAC->next = NULL;
    return newTAC;
}


// Function to replace variables with their corresponding temp registers
void replaceVariablesWithTemp(TAC** head) {
    TAC* current = *head;
    TAC* cleanedHead = NULL;

    // Dictionary to map variables to their corresponding temp vars
    char* variableMap[100][2]; // Adjust size as needed
    int mapIndex = 0;

    while (current != NULL) {
        // If the current operation is a MOV operation assigning a temp var to a variable, update the map
        if (strcmp(current->operation, "MOV") == 0 && isVariable(current->result) && isTemp(current->operand1)) {
            // Store or update the mapping of the variable to its corresponding temp var
            int found = 0;
            for (int i = 0; i < mapIndex; i++) {
                if (strcmp(variableMap[i][0], current->result) == 0) {
                    free(variableMap[i][1]);
                    variableMap[i][1] = strdup(current->operand1);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                variableMap[mapIndex][0] = strdup(current->result);   // Variable name
                variableMap[mapIndex][1] = strdup(current->operand1); // Corresponding temp var
                mapIndex++;
            }

            // Skip adding this MOV to the cleaned TAC since it's just a mapping operation
            current = current->next;
            continue;
        }

        // Replace variable references in operands with their corresponding temp vars from the map
        char* operand1 = current->operand1;
        char* operand2 = current->operand2;
        for (int i = 0; i < mapIndex; i++) {
            if (operand1 && strcmp(operand1, variableMap[i][0]) == 0) {
                operand1 = variableMap[i][1];
            }
            if (operand2 && strcmp(operand2, variableMap[i][0]) == 0) {
                operand2 = variableMap[i][1];
            }
        }

        // Special handling for WRITE operations
        if (strcmp(current->operation, "WRITE") == 0) {
            // If the WRITE operand is a variable, replace it with the corresponding temp register
            for (int i = 0; i < mapIndex; i++) {
                if (current->result && strcmp(current->result, variableMap[i][0]) == 0) {
                    current->result = variableMap[i][1];
                    break;
                }
            }
        }

        // Create a new TAC with the replaced operands
        TAC* newTAC = createTAC(current->operation, current->result, operand1, operand2);

        // Append the new TAC to the cleaned list
        appendTAC(&cleanedHead, newTAC);

        current = current->next;
    }

    // Update the original head to point to the cleaned TAC list
    *head = cleanedHead;
}


// Function to check if a string is a valid variable name
bool isVariable(const char* str) {
    if (str == NULL || *str == '\0') return false;
    if (!isalpha((unsigned char)*str) && *str != '_') return false;
    ++str;
    while (*str) {
        if (!isalnum((unsigned char)*str) && *str != '_') return false;
        ++str;
    }
    return true;
}

// Function to check if a string is a temporary variable (e.g., "t0", "t1", etc.)
bool isTemp(const char* str) {
    return str && str[0] == 't' && isdigit((unsigned char)str[1]);
}

// Function to check if a string is a constant integer
bool isConstant(const char* str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '-') ++str;  // Handle negative constants
    while (*str) {
        if (!isdigit((unsigned char)*str)) return false;
        ++str;
    }
    return true;
}

// Create a temporary variable for TAC results
char* createTempVar() {
    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10);
    snprintf(tempVar, 10, "t%d", tempCounter++);
    return tempVar;
}

// Function to safely append a TAC node to the list
void appendTAC(TAC** head, TAC* newInstruction) {
    if (!newInstruction) return; // Prevent appending a NULL instruction
    if (!*head) {
        *head = newInstruction;
    } else {
        TAC* current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = newInstruction;
    }
}