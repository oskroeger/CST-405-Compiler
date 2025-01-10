#include "symbolTable.h"
#include <stdlib.h>
#include <stddef.h>  // Include the header file for NULL macro
#include <string.h>
#include <stdio.h>

#define TABLE_SIZE 100

SymbolTable* currentScope = NULL; // Initialize the current scope to NULL
ScopeNode* scopeListHead = NULL; // Head of the list of all scopes

SymbolTable* createSymbolTable(SymbolTable* parent, int size) {
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!newTable) {
        fprintf(stderr, "[ERROR] Memory allocation failed for SymbolTable.\n");
        return NULL;
    }

    newTable->size = size;
    newTable->table = (Symbol**)calloc(size, sizeof(Symbol*));
    if (!newTable->table) {
        fprintf(stderr, "[ERROR] Memory allocation failed for SymbolTable array.\n");
        free(newTable);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        if (newTable->table[i] != NULL) {
            fprintf(stderr, "[ERROR] Non-NULL entry in symbol table initialization at index %d.\n", i);
        }
        newTable->table[i] = NULL;
    }

    newTable->parent = parent;
    printf("[DEBUG] Created a new symbol table with size %d.\n", size);
    return newTable;
}

// Hash function to map a name to an index
unsigned int hash(SymbolTable* table, char* name) {
    unsigned int hashval = 0;
    for (; *name != '\0'; name++) hashval = *name + (hashval << 5) - hashval;
    return hashval % table->size;
}

void registerScope(SymbolTable* scope) {
    if (scope == NULL || scope->table == NULL) {
        fprintf(stderr, "[ERROR] Trying to register a NULL or uninitialized scope.\n");
        return;
    }

    ScopeNode* newNode = (ScopeNode*)malloc(sizeof(ScopeNode));
    if (newNode == NULL) {
        fprintf(stderr, "[ERROR] Memory allocation failed in registerScope.\n");
        exit(EXIT_FAILURE);
    }

    newNode->scope = scope;
    newNode->next = NULL;

    if (scopeListHead == NULL) {
        scopeListHead = newNode;
    } else {
        ScopeNode* current = scopeListHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void enterScope() {
    SymbolTable* newScope = createSymbolTable(currentScope, TABLE_SIZE);
    if (newScope == NULL || newScope->table == NULL) {
        fprintf(stderr, "[ERROR] Failed to enter a new scope due to symbol table initialization error.\n");
        return;
    }

    currentScope = newScope;
    registerScope(currentScope);

    if (currentScope == NULL || currentScope->table == NULL) {
        fprintf(stderr, "[ERROR] currentScope or its table array is NULL after entering new scope.\n");
        exit(EXIT_FAILURE);
    }
}

// Function to exit the current scope
void exitScope() {
    if (currentScope) {
        currentScope = currentScope->parent;
        // Do not free the symbol table here
        // freeSymbolTable(oldScope);
    } else {
        fprintf(stderr, "No scope to exit.\n");
    }
}

// Function to add a symbol to the table (for regular variables)
void addSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    // Check for an existing symbol with the same name in the current scope
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            // Update existing symbol's type and value
            sym->type = type;
            if (type == TYPE_INT) {
                sym->value.intValue = value.intValue;
            } else if (type == TYPE_FLOAT) {
                sym->value.floatValue = value.floatValue;
            } else if (type == TYPE_CHAR) {
                sym->value.charValue = value.charValue;
            } else {
                fprintf(stderr, "Warning: Unknown symbol type for symbol '%s'.\n", name);
            }
            return;
        }
    }

    // If no existing symbol found, add a new symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->size = 0;  // Default size for non-array symbols
    newSymbol->isParameter = 0; // Regular variable

    if (type == TYPE_INT) {
        newSymbol->value.intValue = value.intValue;
    } else if (type == TYPE_FLOAT) {
        newSymbol->value.floatValue = value.floatValue;
    }

    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

// Function to add an array symbol to the table
void addArraySymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value, int size) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    // Check for an existing symbol with the same name
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            sym->type = type;
            sym->size = size;
            if (type == TYPE_INT) {
                sym->value.intArray = value.intArray;
            } else if (type == TYPE_FLOAT) {
                sym->value.floatArray = value.floatArray;
            }
            return;
        }
    }

    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->size = size;

    if (type == TYPE_INT) {
        newSymbol->value.intArray = (int*)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            newSymbol->value.intArray[i] = 0;
        }
    } else if (type == TYPE_FLOAT) {
        newSymbol->value.floatArray = (float*)malloc(size * sizeof(float));
        for (int i = 0; i < size; i++) {
            newSymbol->value.floatArray[i] = 0.0;
        }
    }

    newSymbol->isParameter = 0; // Regular array variable
    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

void addFunctionSymbol(SymbolTable* table, char* name) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            sym->type = TYPE_FUNCTION;
            return;
        }
    }

    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = TYPE_FUNCTION;
    newSymbol->size = 0;

    memset(&(newSymbol->value), 0, sizeof(SymbolValue));
    newSymbol->isParameter = 0; // Function symbol is not a parameter

    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

// New function to add parameter symbols
void addParameterSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    // Check for an existing symbol with the same name in the current scope
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            // Update existing symbol's type and value, set isParameter
            sym->type = type;
            sym->isParameter = 1;
            if (type == TYPE_INT) {
                sym->value.intValue = value.intValue;
            } else if (type == TYPE_FLOAT) {
                sym->value.floatValue = value.floatValue;
            }
            return;
        }
    }

    // If no existing symbol found, add a new symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->size = 0;  // Default size for non-array symbols
    newSymbol->isParameter = 1; // Mark as parameter

    if (type == TYPE_INT) {
        newSymbol->value.intValue = value.intValue;
    } else if (type == TYPE_FLOAT) {
        newSymbol->value.floatValue = value.floatValue;
    }

    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

Symbol* lookupSymbol(SymbolTable* table, char* name) {
    if (table == NULL || name == NULL) {
        fprintf(stderr, "[ERROR] lookupSymbol called with NULL arguments.\n");
        return NULL;
    }

    unsigned int hashval = hash(table, name);
    SymbolTable* scope = table;

    // Traverse up the scope chain
    while (scope != NULL) {
        // Check the current scope's symbol table
        for (Symbol* sym = scope->table[hashval]; sym != NULL; sym = sym->next) {
            if (strcmp(name, sym->name) == 0) {
                return sym;  // Found the symbol in the current or a parent scope
            }
        }

        // Move up to the parent scope
        scope = scope->parent;
    }

    // If not found, return NULL
    return NULL;
}

// Function to free the symbol table and its symbols
void freeSymbolTable(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != NULL) {
            Symbol* nextSym = sym->next;
            free(sym->name);
            if ((sym->type == TYPE_INT || sym->type == TYPE_FLOAT) && sym->size > 0) {
                if (sym->type == TYPE_INT) {
                    free(sym->value.intArray);
                } else if (sym->type == TYPE_FLOAT) {
                    free(sym->value.floatArray);
                }
            }
            free(sym);
            sym = nextSym;
        }
    }
    free(table->table);
    free(table);
}

// Enhanced function to print the symbol table for debugging using ScopeNode list
void printSymbolTable(const ScopeNode* scopeListHead) {
    if (scopeListHead == NULL) {
        fprintf(stderr, "[ERROR] Scope list head is NULL. No scopes registered.\n");
        return;
    }

    int scopeLevel = 0;
    const ScopeNode* currentNode = scopeListHead;

    printf("\n\n----- SYMBOL TABLE -----\n");

    // Traverse the linked list of scopes
    while (currentNode != NULL) {
        if (currentNode->scope == NULL) {
            fprintf(stderr, "[ERROR] Current scope is NULL at level %d. Skipping to next scope.\n", scopeLevel);
            currentNode = currentNode->next;
            scopeLevel++;
            continue;
        }

        SymbolTable* currentScope = currentNode->scope;

        // Check if the symbol table is properly initialized
        if (currentScope->table == NULL) {
            fprintf(stderr, "[ERROR] Symbol table array is NULL at scope level %d.\n", scopeLevel);
            currentNode = currentNode->next;
            scopeLevel++;
            continue;
        }

        printf("\nScope Level: %d\n", scopeLevel);
        printf("------------------------\n");

        int hasSymbols = 0;

        // Iterate over the hash table buckets
        for (int i = 0; i < currentScope->size; i++) {
            Symbol* sym = currentScope->table[i];

            // Check if the current bucket is empty
            if (sym == NULL) {
                // printf("[INFO] Bucket %d is empty.\n", i);
                continue;
            }

            // Traverse the linked list of symbols in the current bucket
            while (sym != NULL) {
                hasSymbols = 1;

                // Diagnostic check for symbol integrity
                if (sym->name == NULL) {
                    fprintf(stderr, "[ERROR] Symbol name is NULL in bucket %d at scope level %d. Skipping symbol.\n", i, scopeLevel);
                    sym = sym->next;
                    continue;
                }

                // Print symbol information based on type
                if (sym->type == TYPE_FUNCTION) {
                    printf("Name: %s | Type: FUNCTION\n", sym->name);
                } else if (sym->size > 0) { // Array type
                    printf("Name: %s | Type: %s | Array Size: %d\n", sym->name, symbolTypeToString(sym->type), sym->size);

                    // Check for array data integrity
                    if (sym->type == TYPE_INT && sym->value.intArray == NULL) {
                        fprintf(stderr, "[ERROR] Int array is NULL for symbol '%s'.\n", sym->name);
                    } else if (sym->type == TYPE_FLOAT && sym->value.floatArray == NULL) {
                        fprintf(stderr, "[ERROR] Float array is NULL for symbol '%s'.\n", sym->name);
                    }

                    // Print array contents
                    if (sym->type == TYPE_INT && sym->value.intArray != NULL) {
                        for (int j = 0; j < sym->size; j++) {
                            printf("\tIndex %d: %d\n", j, sym->value.intArray[j]);
                        }
                    } else if (sym->type == TYPE_FLOAT && sym->value.floatArray != NULL) {
                        for (int j = 0; j < sym->size; j++) {
                            printf("\tIndex %d: %.4f\n", j, sym->value.floatArray[j]);
                        }
                    }
                } else { // Regular variable
                    switch (sym->type) {
                        case TYPE_INT:
                            printf("Name: %s | Type: INT | Value: %d | %s\n", 
                                   sym->name, sym->value.intValue, sym->isParameter ? "(Parameter)" : "");
                            break;
                        case TYPE_FLOAT:
                            printf("Name: %s | Type: FLOAT | Value: %.4f | %s\n", 
                                   sym->name, sym->value.floatValue, sym->isParameter ? "(Parameter)" : "");
                            break;
                        case TYPE_CHAR:
                            printf("Name: %s | Type: CHAR | Value: %c | %s\n", 
                                   sym->name, sym->value.charValue, sym->isParameter ? "(Parameter)" : "");
                            break;
                        default:
                            printf("Name: %s | Type: UNKNOWN\n", sym->name);
                            break;
                    }
                }

                // Move to the next symbol in the linked list
                sym = sym->next;
            }
        }

        // If no symbols were found in the current scope, indicate that
        if (!hasSymbols) {
            printf("No symbols in this scope.\n");
        }

        // Move to the next scope in the linked list
        currentNode = currentNode->next;
        scopeLevel++;
    }

    printf("\n------------------------\n");
}

// Helper function to convert SymbolType to a string
const char* symbolTypeToString(SymbolType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_FUNCTION: return "function";
        case TYPE_UNKNOWN: return "unknown";
        default: return "undefined";
    }
}
