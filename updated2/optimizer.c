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


// Main optimization function
void optimizeTAC(TAC** head) {
    TAC* optimizedHead = NULL;

    // Apply constant folding
    constantFolding(head, &optimizedHead);

    // Apply copy propagation
    TAC* propagatedHead = NULL;
    copyPropagation(&optimizedHead, &propagatedHead);

    // Apply dead code elimination
    TAC* finalOptimizedHead = NULL;
    deadCodeElimination(&propagatedHead, &finalOptimizedHead);

    // Renumber registers to maintain a clean sequence
    renumberRegisters(&finalOptimizedHead);

    // Update the original head to point to the fully optimized TAC
    *head = finalOptimizedHead;
}


// Function to fold constant expressions in the TAC
void constantFolding(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;

    // Map to track which temp registers hold constant values
    char* constantMap[100] = {NULL};  // Map to store constants as strings for each temp register

    while (current != NULL) {

        // Track constants loaded into temp registers
        if (strcmp(current->operation, "MOV") == 0 && isConstant(current->operand1)) {
            // Store the constant value in the map
            int index = atoi(&current->result[1]);  // Assuming temp vars are in format t0, t1, etc.
            constantMap[index] = strdup(current->operand1);
        }

        // Fold arithmetic operations with constants
        if (strcmp(current->operation, "ADD") == 0 || strcmp(current->operation, "SUB") == 0 ||
            strcmp(current->operation, "MUL") == 0 || strcmp(current->operation, "DIV") == 0) {

            // Check if operands are temp registers holding constants
            //int leftIndex = (current->operand1 && current->operand1[0] == 't') ? atoi(&current->operand1[1]) : -1;
            //int rightIndex = (current->operand2 && current->operand2[0] == 't') ? atoi(&current->operand2[1]) : -1;

            int leftIndex = (current->operand1 && (current->operand1[0] == 't' || current->operand1[0] == 'f')) ? atoi(&current->operand1[1]) : -1;

            int rightIndex = (current->operand2 && (current->operand2[0] == 't' || current->operand2[0] == 'f')) ? atoi(&current->operand2[1]) : -1;

            // Fold if both are constants
            /*if (leftIndex != -1 && rightIndex != -1 && constantMap[leftIndex] && constantMap[rightIndex]) {
                double leftValue, rightValue, result;

                // Use atof for float registers, atoi for integer registers
                if (current->operand1[0] == 'f') {
                    leftValue = atof(constantMap[leftIndex]);
                    rightValue = atof(constantMap[rightIndex]);
                } else {
                    leftValue = atoi(constantMap[leftIndex]);
                    rightValue = atoi(constantMap[rightIndex]);
                }

                // Perform the operation
                if (strcmp(current->operation, "ADD") == 0) {
                    result = leftValue + rightValue;
                } else if (strcmp(current->operation, "SUB") == 0) {
                    result = leftValue - rightValue;
                } else if (strcmp(current->operation, "MUL") == 0) {
                    result = leftValue * rightValue;
                } else if (strcmp(current->operation, "DIV") == 0 && rightValue != 0) {
                    result = leftValue / rightValue;
                } else if (strcmp(current->operation, "DIV") == 0 && rightValue == 0) {
                    fprintf(stderr, "Error: Division by zero.\n");
                    exit(EXIT_FAILURE);
                }

                // Convert the result back to a string and create a new MOV TAC
                char resultStr[20];
                snprintf(resultStr, sizeof(resultStr), current->operand1[0] == 'f' ? "%f" : "%d", result);
                TAC* newTAC = createTAC("MOV", current->result, resultStr, NULL);
                appendTAC(optimizedHead, newTAC);

                // Update the constant map
                int index = atoi(&current->result[1]);
                constantMap[index] = strdup(resultStr);
            } else {
                // If not folding, just append the current TAC to the new list
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
                appendTAC(optimizedHead, newTAC);
            }
        } else {
            // Append non-arithmetic operations without modification
            TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
            appendTAC(optimizedHead, newTAC);
        }

        current = current->next;
    }

    // Print the TAC after constant folding to ensure correctness
    printf("\n----- TAC After Constant Folding -----\n");
    TAC* printCurrent = *optimizedHead;
    while (printCurrent != NULL) {
        printf("%s = %s %s %s\n", printCurrent->result, 
               printCurrent->operand1 ? printCurrent->operand1 : "", 
               printCurrent->operation, 
               printCurrent->operand2 ? printCurrent->operand2 : "");
        printCurrent = printCurrent->next;
    }
}*/
            // Fold if both are constants
            if (leftIndex != -1 && rightIndex != -1 && constantMap[leftIndex] && constantMap[rightIndex]) {
                int leftValue = atoi(constantMap[leftIndex]);
                int rightValue = atoi(constantMap[rightIndex]);
                int result = 0;

                // Handle each operation
                if (strcmp(current->operation, "ADD") == 0) {
                    result = leftValue + rightValue;
                } else if (strcmp(current->operation, "SUB") == 0) {
                    result = leftValue - rightValue;
                } else if (strcmp(current->operation, "MUL") == 0) {
                    result = leftValue * rightValue;
                } else if (strcmp(current->operation, "DIV") == 0 && rightValue != 0) {
                    result = leftValue / rightValue;
                } else if (strcmp(current->operation, "DIV") == 0 && rightValue == 0) {
                    fprintf(stderr, "Error: Division by zero.\n");
                    exit(EXIT_FAILURE);
                }

                // Convert the result back to a string and create a new MOV TAC
                char resultStr[20];
                snprintf(resultStr, sizeof(resultStr), "%d", result);
                TAC* newTAC = createTAC("MOV", current->result, resultStr, NULL);
                appendTAC(optimizedHead, newTAC);

                // Update the constant map
                int index = atoi(&current->result[1]);
                constantMap[index] = strdup(resultStr);
            } else {
                // If not folding, just append the current TAC to the new list
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
                appendTAC(optimizedHead, newTAC);
            }
        } else {
            // Append non-arithmetic operations without modification
            TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
            appendTAC(optimizedHead, newTAC);
        }

        current = current->next;
    } 

    // Print the TAC after constant folding to ensure correctness
    printf("\n----- TAC After Constant Folding -----\n");
    TAC* printCurrent = *optimizedHead;
    while (printCurrent != NULL) {
        printf("%s = %s %s %s\n", printCurrent->result, 
               printCurrent->operand1 ? printCurrent->operand1 : "", 
               printCurrent->operation, 
               printCurrent->operand2 ? printCurrent->operand2 : "");
        printCurrent = printCurrent->next;
    }
}

// Function to propagate copies and eliminate unnecessary MOVs
void copyPropagation(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;
    char* copyMap[100] = {NULL};  // To track which temp holds the value of another temp

    while (current != NULL) {
        // Check for MOV operations where one temp copies another
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->operand1)) {
            // Track that the current temp result holds the same value as operand1
            int index = atoi(&current->result[1]);
            copyMap[index] = strdup(current->operand1);

        } else if (strcmp(current->operation, "WRITE") == 0) {
            // Do NOT propagate copies into WRITE operations, use the original temp
            TAC* newTAC = createTAC(current->operation, current->result, NULL, NULL);
            appendTAC(optimizedHead, newTAC);

        } else {
            // Replace operands with their propagated values if they exist
            char* operand1 = current->operand1;
            char* operand2 = current->operand2;

            int leftIndex = (operand1 && (operand1[0] == 't' || operand1[0] == 'f')) ? atoi(&operand1[1]) : -1;

            int rightIndex = (operand2 && (operand2[0] == 't' || operand2[0] == 'f')) ? atoi(&operand2[1]) : -1;

            // If the temp has been mapped, replace it with the value from copyMap
            if (leftIndex != -1 && copyMap[leftIndex]) {
                operand1 = copyMap[leftIndex];
            }
            if (rightIndex != -1 && copyMap[rightIndex]) {
                operand2 = copyMap[rightIndex];
            }

            // Create a new TAC with updated values
            TAC* newTAC = createTAC(current->operation, current->result, operand1, operand2);
            appendTAC(optimizedHead, newTAC);
        }

        current = current->next;
    }

    // Print the TAC after copy propagation for verification
    printf("\n----- TAC After Copy Propagation -----\n");
    TAC* temp = *optimizedHead;
    while (temp != NULL) {
        printf("%s = %s %s %s\n", temp->result, 
               temp->operand1 ? temp->operand1 : "", 
               temp->operation, 
               temp->operand2 ? temp->operand2 : "");
        temp = temp->next;
    }
}


// Function to eliminate dead code (unnecessary instructions)
void deadCodeElimination(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;
    int usedRegs[100] = {0};  // Track used registers
    int hasWrites = 0;        // Flag to check if there are any WRITE statements

    // First pass: mark registers used in WRITE operations and binary operations
    while (current != NULL) {
        if (strcmp(current->operation, "WRITE") == 0 && current->result) {
            // Mark the register or variable used in the WRITE operation
            if (current->result[0] == 't') {
                int regIndex = atoi(&current->result[1]);
                usedRegs[regIndex] = 1;  // Mark the temp register as used
            }
            hasWrites = 1;           // Mark that we have WRITE operations
        } else if (isBinaryOperation(current->operation)) {
            // For binary operations (ADD, SUB, etc.), mark both operands as used
            if (current->operand1 && current->operand1[0] == 't') {
                int regIndex = atoi(&current->operand1[1]);
                usedRegs[regIndex] = 1;
            }
            if (current->operand2 && current->operand2[0] == 't') {
                int regIndex = atoi(&current->operand2[1]);
                usedRegs[regIndex] = 1;
            }
        }
        current = current->next;
    }

    // Second pass: copy only necessary instructions, preserving WRITE operations
    current = *head;
    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->result)) {
            int regIndex = atoi(&current->result[1]);
            if (usedRegs[regIndex]) {
                // Append only if the register is marked as used
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
                appendTAC(optimizedHead, newTAC);
            }
        } else if (isBinaryOperation(current->operation)) {
            // For binary operations, append the instruction if any operand is marked as used
            int leftReg = (current->operand1 && current->operand1[0] == 't') ? atoi(&current->operand1[1]) : -1;
            int rightReg = (current->operand2 && current->operand2[0] == 't') ? atoi(&current->operand2[1]) : -1;

            if ((leftReg != -1 && usedRegs[leftReg]) || (rightReg != -1 && usedRegs[rightReg])) {
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
                appendTAC(optimizedHead, newTAC);
            }
        } else if (strcmp(current->operation, "WRITE") == 0) {
            // Ensure the WRITE operation retains its correct reference
            TAC* newTAC = createTAC(current->operation, current->result, current->result, NULL);
            appendTAC(optimizedHead, newTAC);
        } else {
            // Append other operations (including WRITE)
            TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
            appendTAC(optimizedHead, newTAC);
        }
        current = current->next;
    }

    // If no WRITE statements were found and the optimized TAC is empty, append the last instruction
    if (!hasWrites && *optimizedHead == NULL && *head != NULL) {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2);
        appendTAC(optimizedHead, newTAC);
    }

    // Print final TAC after dead code elimination
    printf("\n----- TAC After Dead Code Elimination -----\n");
    TAC* temp = *optimizedHead;
    while (temp != NULL) {
        printf("%s = %s %s %s\n", temp->result, temp->operand1, temp->operation, temp->operand2 ? temp->operand2 : "");
        temp = temp->next;
    }
}



// Helper function to check if an operation is a binary arithmetic operation
bool isBinaryOperation(const char* operation) {
    return (strcmp(operation, "ADD") == 0 || strcmp(operation, "SUB") == 0 ||
            strcmp(operation, "MUL") == 0 || strcmp(operation, "DIV") == 0);
}

// Helper function to renumber registers while preserving WRITE references
void renumberRegisters(TAC** head) {
    TAC* current = *head;
    TAC* newHead = NULL;
    TAC* newTail = NULL;

    // Map to track old registers to new register numbers
    char* registerMap[100] = {NULL};  // Adjust size as needed
    int regCounter = 0;

    // First Pass: Renumber MOV operations, ignoring WRITE
    while (current != NULL) {
        char newResult[10] = "";
        char newOperand1[10] = "";
        char newOperand2[10] = "";

        // Renumber result register and update the map
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->result)) {
            int index = atoi(&current->result[1]);
            if (!registerMap[index]) {
                sprintf(newResult, "t%d", regCounter);
                registerMap[index] = strdup(newResult);
                // printf("Mapping %s -> %s\n", current->result, newResult);  // Debug print
                regCounter++;
            } else {
                strcpy(newResult, registerMap[index]);
            }
        } else {
            strcpy(newResult, current->result ? current->result : "");
        }

        // Renumber operand1 register if it exists in the map
        if (current->operand1 && isTemp(current->operand1)) {
            int oldIndex = atoi(&current->operand1[1]);
            strcpy(newOperand1, registerMap[oldIndex] ? registerMap[oldIndex] : current->operand1);
        } else {
            strcpy(newOperand1, current->operand1 ? current->operand1 : "");
        }

        // Renumber operand2 register if it exists in the map
        if (current->operand2 && isTemp(current->operand2)) {
            int oldIndex = atoi(&current->operand2[1]);
            strcpy(newOperand2, registerMap[oldIndex] ? registerMap[oldIndex] : current->operand2);
        } else {
            strcpy(newOperand2, current->operand2 ? current->operand2 : "");
        }

        // Create the new TAC and append it to the new list
        TAC* newTAC = createTAC(current->operation, newResult, newOperand1, newOperand2);
        if (newHead == NULL) {
            newHead = newTAC;
            newTail = newTAC;
        } else {
            newTail->next = newTAC;
            newTail = newTAC;
        }

        current = current->next;
    }

    // Second Pass: Update WRITE instructions to ensure direct mapping and clean references
    current = newHead;
    while (current != NULL) {
        if (strcmp(current->operation, "WRITE") == 0 && isTemp(current->result)) {
            int writeRegIndex = atoi(&current->result[1]);
            if (registerMap[writeRegIndex]) {
                // Apply the direct mapping from the register map without the redundant reference
                strcpy(current->result, registerMap[writeRegIndex]);
                current->operand1 = NULL;  // Remove redundant operand reference
                // printf("Correct WRITE mapping to: %s\n", current->result);  // Debug print
            }
        }
        current = current->next;
    }

    // Update the original head to point to the renumbered TAC list
    *head = newHead;

    // Clean up memory used by registerMap
    for (int i = 0; i < 100; i++) {
        if (registerMap[i]) {
            free(registerMap[i]);
        }
    }
}



// Function to replace variables with their corresponding temp registers and handle WRITE operations
void replaceVariablesWithTemp(TAC** head) {
    TAC* current = *head;
    TAC* cleanedHead = NULL;
    TAC* cleanedTail = NULL;

    // Dictionary to map variables to their corresponding temp vars or other variables
    char* variableMap[100][2]; // Adjust size as needed
    int mapIndex = 0;

    while (current != NULL) {
        // Handle MOV operations involving variables or variable-to-variable assignments
        if (strcmp(current->operation, "MOV") == 0 && (isVariable(current->result) && (isTemp(current->operand1) || isVariable(current->operand1)))) {
            // If the operand is a variable, resolve it to its temp register or mapped variable
            char* finalOperand = current->operand1;
            for (int i = 0; i < mapIndex; i++) {
                if (strcmp(finalOperand, variableMap[i][0]) == 0) {
                    finalOperand = variableMap[i][1];  // Resolve to the mapped value
                    break;
                }
            }

            // Update the mapping of the variable to its corresponding temp var or resolved variable
            int found = 0;
            for (int i = 0; i < mapIndex; i++) {
                if (strcmp(variableMap[i][0], current->result) == 0) {
                    free(variableMap[i][1]);
                    variableMap[i][1] = strdup(finalOperand);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                variableMap[mapIndex][0] = strdup(current->result);   // Variable name
                variableMap[mapIndex][1] = strdup(finalOperand); // Corresponding temp var or resolved variable
                mapIndex++;
            }
            current = current->next;
            continue;
        }

        // Replace variable references in operands with corresponding temp vars or resolved variables
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

        // Handle WRITE operations, ensuring correct referencing
        if (strcmp(current->operation, "WRITE") == 0) {
            for (int i = 0; i < mapIndex; i++) {
                if (current->result && strcmp(current->result, variableMap[i][0]) == 0) {
                    current->result = variableMap[i][1];
                    break;
                }
            }
        }

        // Create a new TAC node
        TAC* newTAC = createTAC(current->operation, current->result, operand1, operand2);

        // Append to the cleaned list
        if (cleanedHead == NULL) {
            cleanedHead = newTAC;
            cleanedTail = newTAC;
        } else {
            cleanedTail->next = newTAC;
            cleanedTail = newTAC;
        }

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
    return (str && str[0] == 't' && isdigit((unsigned char)str[1])) || (str && str[0] == 'f' && isdigit((unsigned char)str[1]));
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
    static int tempTCounter = 0;
    static int tempFCounter = 0;
    char* tempVar = (char*)malloc(10);
    // if tempVAr is a float use f register
    if (tempVar[0] == 'f') {
        snprintf(tempVar, 10, "f%d", tempFCounter++);
    } else {
        snprintf(tempVar, 10, "t%d", tempTCounter++); }
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