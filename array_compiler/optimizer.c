#include "optimizer.h"
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to create a new TAC node
TAC* createTAC(const char* operation, const char* result, const char* operand1, const char* operand2, SymbolType type) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    if (!newTAC) {
        fprintf(stderr, "Memory allocation failed for new TAC node.\n");
        exit(EXIT_FAILURE);
    }
    newTAC->operation = strdup(operation);
    newTAC->result = result ? strdup(result) : NULL;
    newTAC->operand1 = operand1 ? strdup(operand1) : NULL;
    newTAC->operand2 = operand2 ? strdup(operand2) : NULL;
    newTAC->type = type;  // Store the type of the operation (int or float)
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

void constantFolding(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;

    // Map to track which temp registers hold constant values
    char* constantMap[100] = {NULL};  // Map to store constants as strings for each temp register
    SymbolType typeMap[100];          // Map to store the type (int or float) for each temp register

    while (current != NULL) {

        // Track constants loaded into temp registers
        if (strcmp(current->operation, "MOV") == 0 && isConstant(current->operand1)) {
            // Store the constant value and its type (int or float) in the map
            int index = atoi(&current->result[1]);  // Extracting index from temp vars t0, t1, etc.
            constantMap[index] = strdup(current->operand1);
            typeMap[index] = isFloatConstant(current->operand1) ? TYPE_FLOAT : TYPE_INT;  // Determine type based on constant
        }

        // Fold arithmetic operations with constants
        if (strcmp(current->operation, "ADD") == 0 || strcmp(current->operation, "SUB") == 0 ||
            strcmp(current->operation, "MUL") == 0 || strcmp(current->operation, "DIV") == 0) {

            // Check if operands are temp registers holding constants
            int leftIndex = (current->operand1 && (current->operand1[0] == 't' || current->operand1[0] == 'f')) ? atoi(&current->operand1[1]) : -1;
            int rightIndex = (current->operand2 && (current->operand2[0] == 't' || current->operand2[0] == 'f')) ? atoi(&current->operand2[1]) : -1;

            // Fold if both are constants of the same or compatible types
            if (leftIndex != -1 && rightIndex != -1 && constantMap[leftIndex] && constantMap[rightIndex]) {
                if (typeMap[leftIndex] == TYPE_INT && typeMap[rightIndex] == TYPE_INT) {
                    // Integer folding
                    int leftValue = atoi(constantMap[leftIndex]);
                    int rightValue = atoi(constantMap[rightIndex]);
                    int result = 0;

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
                    TAC* newTAC = createTAC("MOV", current->result, resultStr, NULL, TYPE_INT);
                    appendTAC(optimizedHead, newTAC);

                    // Update the constant map and type map
                    constantMap[atoi(&current->result[1])] = strdup(resultStr);
                    typeMap[atoi(&current->result[1])] = TYPE_INT;
                } else {
                    // Float folding (if either operand is float)
                    float leftValue = atof(constantMap[leftIndex]);
                    float rightValue = atof(constantMap[rightIndex]);
                    float result = 0.0;

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
                    snprintf(resultStr, sizeof(resultStr), "%.6f", result);  // Ensuring precision for floats
                    TAC* newTAC = createTAC("MOV", current->result, resultStr, NULL, TYPE_FLOAT);  // Keep it as TYPE_FLOAT
                    appendTAC(optimizedHead, newTAC);

                    // Update the constant map and type map
                    constantMap[atoi(&current->result[1])] = strdup(resultStr);
                    typeMap[atoi(&current->result[1])] = TYPE_FLOAT;
                }
            } else {
                // If not folding, just append the current TAC to the new list
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
                appendTAC(optimizedHead, newTAC);
            }
        } else {
            // Append non-arithmetic operations without modification
            TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
            appendTAC(optimizedHead, newTAC);
        }

        current = current->next;
    }

    // Print the TAC after constant folding for debugging
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


void copyPropagation(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;
    char* copyMap[100] = {NULL};    // To track which temp holds the value of another temp

    while (current != NULL) {
        // Check for MOV operations where one temp copies another
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->operand1)) {
            // Track that the current temp result holds the same value as operand1
            int index = atoi(&current->result[1]);
            copyMap[index] = strdup(current->operand1); // Save the copied operand
        } else if (strcmp(current->operation, "WRITE") == 0) {
            // Do NOT propagate copies into WRITE operations, use the original temp
            TAC* newTAC = createTAC(current->operation, current->result, NULL, NULL, current->type);
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
            TAC* newTAC = createTAC(current->operation, current->result, operand1, operand2, current->type);
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

    // Clean up copyMap to free allocated memory
    for (int i = 0; i < 100; i++) {
        if (copyMap[i]) {
            free(copyMap[i]);
        }
    }
}


void deadCodeElimination(TAC** head, TAC** optimizedHead) {
    TAC* current = *head;
    int usedRegs[100] = {0};        // Track used registers for integers
    int usedFloatRegs[100] = {0};   // Track used registers for floats
    int hasWrites = 0;              // Flag to check if there are any WRITE statements

    // First pass: mark registers used in WRITE operations and binary operations
    while (current != NULL) {
        if (strcmp(current->operation, "WRITE") == 0 && current->result) {
            // Mark the register or variable used in the WRITE operation
            if (current->result[0] == 't') {
                int regIndex = atoi(&current->result[1]);
                usedRegs[regIndex] = 1;  // Mark the integer temp register as used
            } else if (current->result[0] == 'f') {
                int regIndex = atoi(&current->result[1]);
                usedFloatRegs[regIndex] = 1;  // Mark the float temp register as used
            }
            hasWrites = 1;  // Mark that we have WRITE operations
        } else if (isBinaryOperation(current->operation)) {
            // For binary operations (ADD, SUB, etc.), mark both operands as used
            if (current->operand1 && current->operand1[0] == 't') {
                int regIndex = atoi(&current->operand1[1]);
                usedRegs[regIndex] = 1;
            } else if (current->operand1 && current->operand1[0] == 'f') {
                int regIndex = atoi(&current->operand1[1]);
                usedFloatRegs[regIndex] = 1;
            }
            if (current->operand2 && current->operand2[0] == 't') {
                int regIndex = atoi(&current->operand2[1]);
                usedRegs[regIndex] = 1;
            } else if (current->operand2 && current->operand2[0] == 'f') {
                int regIndex = atoi(&current->operand2[1]);
                usedFloatRegs[regIndex] = 1;
            }
        }
        current = current->next;
    }

    // Second pass: copy only necessary instructions, preserving WRITE operations
    current = *head;
    while (current != NULL) {
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->result)) {
            int regIndex = atoi(&current->result[1]);
            if ((current->result[0] == 't' && usedRegs[regIndex]) ||
                (current->result[0] == 'f' && usedFloatRegs[regIndex])) {
                // Append only if the register is marked as used
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
                appendTAC(optimizedHead, newTAC);
            }
        } else if (isBinaryOperation(current->operation)) {
            // For binary operations, append the instruction if any operand is marked as used
            int leftReg = (current->operand1 && (current->operand1[0] == 't' || current->operand1[0] == 'f')) ? atoi(&current->operand1[1]) : -1;
            int rightReg = (current->operand2 && (current->operand2[0] == 't' || current->operand2[0] == 'f')) ? atoi(&current->operand2[1]) : -1;

            if ((leftReg != -1 && ((current->operand1[0] == 't' && usedRegs[leftReg]) || 
                                   (current->operand1[0] == 'f' && usedFloatRegs[leftReg]))) ||
                (rightReg != -1 && ((current->operand2[0] == 't' && usedRegs[rightReg]) ||
                                   (current->operand2[0] == 'f' && usedFloatRegs[rightReg])))) {
                TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
                appendTAC(optimizedHead, newTAC);
            }
        } else if (strcmp(current->operation, "WRITE") == 0) {
            // Ensure the WRITE operation retains its correct reference
            TAC* newTAC = createTAC(current->operation, current->result, current->result, NULL, current->type);
            appendTAC(optimizedHead, newTAC);
        } else {
            // Append other operations (including WRITE)
            TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
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
        TAC* newTAC = createTAC(current->operation, current->result, current->operand1, current->operand2, current->type);
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

    // Maps to track old registers to new register numbers for integers (t) and floats (f)
    char* intRegisterMap[100] = {NULL};  // For integer registers (t0, t1, ...)
    char* floatRegisterMap[100] = {NULL};  // For float registers (f0, f1, ...)
    int intRegCounter = 0;  // Counter for integer temp registers
    int floatRegCounter = 0;  // Counter for float temp registers

    // First Pass: Renumber MOV operations, ignoring WRITE
    while (current != NULL) {
        char newResult[10] = "";
        char newOperand1[10] = "";
        char newOperand2[10] = "";

        // Renumber result register and update the map based on register type (int or float)
        if (strcmp(current->operation, "MOV") == 0 && isTemp(current->result)) {
            int index = atoi(&current->result[1]);  // Get the register index number

            if (current->result[0] == 't') {  // Integer register renumbering
                if (!intRegisterMap[index]) {
                    sprintf(newResult, "t%d", intRegCounter);
                    intRegisterMap[index] = strdup(newResult);
                    intRegCounter++;
                } else {
                    strcpy(newResult, intRegisterMap[index]);
                }
            } else if (current->result[0] == 'f') {  // Float register renumbering
                if (!floatRegisterMap[index]) {
                    sprintf(newResult, "f%d", floatRegCounter);
                    floatRegisterMap[index] = strdup(newResult);
                    floatRegCounter++;
                } else {
                    strcpy(newResult, floatRegisterMap[index]);
                }
            }
        } else {
            strcpy(newResult, current->result ? current->result : "");
        }

        // Renumber operand1 register if it exists in the map based on its type
        if (current->operand1 && isTemp(current->operand1)) {
            int oldIndex = atoi(&current->operand1[1]);

            if (current->operand1[0] == 't') {  // Integer operand
                strcpy(newOperand1, intRegisterMap[oldIndex] ? intRegisterMap[oldIndex] : current->operand1);
            } else if (current->operand1[0] == 'f') {  // Float operand
                strcpy(newOperand1, floatRegisterMap[oldIndex] ? floatRegisterMap[oldIndex] : current->operand1);
            }
        } else {
            strcpy(newOperand1, current->operand1 ? current->operand1 : "");
        }

        // Renumber operand2 register if it exists in the map based on its type
        if (current->operand2 && isTemp(current->operand2)) {
            int oldIndex = atoi(&current->operand2[1]);

            if (current->operand2[0] == 't') {  // Integer operand
                strcpy(newOperand2, intRegisterMap[oldIndex] ? intRegisterMap[oldIndex] : current->operand2);
            } else if (current->operand2[0] == 'f') {  // Float operand
                strcpy(newOperand2, floatRegisterMap[oldIndex] ? floatRegisterMap[oldIndex] : current->operand2);
            }
        } else {
            strcpy(newOperand2, current->operand2 ? current->operand2 : "");
        }

        // Create the new TAC and append it to the new list
        TAC* newTAC = createTAC(current->operation, newResult, newOperand1, newOperand2, current->type);
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

            if (current->result[0] == 't' && intRegisterMap[writeRegIndex]) {
                strcpy(current->result, intRegisterMap[writeRegIndex]);
            } else if (current->result[0] == 'f' && floatRegisterMap[writeRegIndex]) {
                strcpy(current->result, floatRegisterMap[writeRegIndex]);
            }
            current->operand1 = NULL;  // Remove redundant operand reference
        }
        current = current->next;
    }

    // Update the original head to point to the renumbered TAC list
    *head = newHead;

    // Print final TAC after renumbering
    printf("\n----- TAC After Renumbering -----\n");
    TAC* temp = *head;
    while (temp != NULL) {
        printf("%s = %s %s %s\n", temp->result, 
               temp->operand1 ? temp->operand1 : "", 
               temp->operation, 
               temp->operand2 ? temp->operand2 : "");
        temp = temp->next;
    }

    // Clean up memory used by register maps
    for (int i = 0; i < 100; i++) {
        if (intRegisterMap[i]) {
            free(intRegisterMap[i]);
        }
        if (floatRegisterMap[i]) {
            free(floatRegisterMap[i]);
        }
    }
}


void replaceVariablesWithTemp(TAC** head) {
    TAC* current = *head;
    TAC* cleanedHead = NULL;
    TAC* cleanedTail = NULL;

    // Dictionary to map variables (including arrays) to their corresponding temp vars or other variables
    char* variableMap[200][2]; // Adjust size to handle both int (t) and float (f) temp vars
    int mapIndex = 0;

    while (current != NULL) {
        // Handle MOV operations involving variables, arrays, or variable-to-variable assignments
        if (strcmp(current->operation, "MOV") == 0 && 
            (isVariable(current->result) || isArrayAccess(current->result)) && 
            (isTemp(current->operand1) || isVariable(current->operand1) || isArrayAccess(current->operand1))) {

            // Resolve the operand (it could be a variable or array access)
            char* finalOperand = current->operand1;

            if (isArrayAccess(finalOperand)) {
                // If it's an array access, resolve it to a temp variable
                char* tempVar = createTempVar(false); // Assuming arrays are of type int
                TAC* newTAC = createTAC("MOV", tempVar, finalOperand, NULL, TYPE_INT);
                appendTAC(&cleanedHead, newTAC);
                finalOperand = tempVar; // Use the temp variable from now on
            } else {
                // If it's not an array access, resolve it to the mapped variable
                for (int i = 0; i < mapIndex; i++) {
                    if (strcmp(finalOperand, variableMap[i][0]) == 0) {
                        finalOperand = variableMap[i][1];  // Resolve to mapped value
                        break;
                    }
                }
            }

            // Update or create the mapping for the variable (or array)
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
                variableMap[mapIndex][0] = strdup(current->result);   // Variable or array name
                variableMap[mapIndex][1] = strdup(finalOperand); // Corresponding temp or resolved variable
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

        // Handle WRITE operations by replacing variable names with temp vars
        if (strcmp(current->operation, "WRITE") == 0) {
            for (int i = 0; i < mapIndex; i++) {
                if (current->result && strcmp(current->result, variableMap[i][0]) == 0) {
                    current->result = variableMap[i][1];
                    break;
                }
            }
        }

        // Create a new TAC node with the updated operands
        TAC* newTAC = createTAC(current->operation, current->result, operand1, operand2, current->type);

        // Append the new node to the cleaned list
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


// Function to check if a string is an array access (e.g., arr[0])
bool isArrayAccess(const char* str) {
    return (str && strchr(str, '[') && strchr(str, ']'));
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

// Function to check if a string is a temporary variable (e.g., "t0", "t1", "f0", etc.)
bool isTemp(const char* str) {
    return (str && ((str[0] == 't' || str[0] == 'f') && isdigit((unsigned char)str[1])));
}


// Function to check if a string is a constant integer or float
bool isConstant(const char* str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '-') ++str;  // Handle negative constants

    bool hasDot = false;
    while (*str) {
        if (*str == '.') {
            if (hasDot) return false;  // Only allow one dot for float numbers
            hasDot = true;
        } else if (!isdigit((unsigned char)*str)) {
            return false;
        }
        ++str;
    }
    return true;
}

// New function to check if a string is specifically a float constant
bool isFloatConstant(const char* str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '-') ++str;  // Handle negative constants

    bool hasDot = false;
    bool hasDigit = false;
    while (*str) {
        if (*str == '.') {
            if (hasDot) return false;  // Only allow one dot for float numbers
            hasDot = true;
        } else if (isdigit((unsigned char)*str)) {
            hasDigit = true;
        } else {
            return false; // Invalid character
        }
        ++str;
    }
    return hasDot && hasDigit; // Must have at least one digit and a dot for it to be a float
}



// Create a temporary variable for TAC results (supports both integers and floats)
char* createTempVar(bool isFloat) {
    static int tempTCounter = 0;
    static int tempFCounter = 0;
    char* tempVar = (char*)malloc(10);  // Allocate memory for the temp variable

    if (isFloat) {
        snprintf(tempVar, 10, "f%d", tempFCounter++);  // Create a float temp variable (e.g., f0, f1, ...)
    } else {
        snprintf(tempVar, 10, "t%d", tempTCounter++);  // Create an integer temp variable (e.g., t0, t1, ...)
    }

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
