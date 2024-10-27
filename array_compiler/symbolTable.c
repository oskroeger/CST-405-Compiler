#include "symbolTable.h"
#include <stdlib.h>
#include <stddef.h>  // Include the header file for NULL macro
#include <string.h>
#include <stdio.h>

// Function to create a new symbol table
SymbolTable* createSymbolTable(int size) {
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!newTable) return NULL;

    newTable->size = size;
    newTable->table = (Symbol**)malloc(sizeof(Symbol*) * size);
    if (!newTable->table) {
        free(newTable);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        newTable->table[i] = NULL;
    }

    return newTable;
}

// Hash function to map a name to an index
unsigned int hash(SymbolTable* table, char* name) {
    unsigned int hashval = 0;
    for (; *name != '\0'; name++) hashval = *name + (hashval << 5) - hashval;
    return hashval % table->size;
}

// Function to add a symbol to the table (for regular variables)
void addSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    // Check for an existing symbol with the same name
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
            // Update existing array symbol's type, value, and size
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

    // If no existing symbol found, add a new array symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->size = size;  // Store the array size

    if (type == TYPE_INT) {
        newSymbol->value.intArray = (int*)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            newSymbol->value.intArray[i] = 0;  // Initialize all elements to 0
        }
    } else if (type == TYPE_FLOAT) {
        newSymbol->value.floatArray = (float*)malloc(size * sizeof(float));
        for (int i = 0; i < size; i++) {
            newSymbol->value.floatArray[i] = 0.0;  // Initialize all elements to 0.0
        }
    }

    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

// Function to look up a name in the table
Symbol* lookupSymbol(SymbolTable* table, char* name) {
    unsigned int hashval = hash(table, name);

    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) return sym;
    }

    return NULL;
}

// Function to free the symbol table
void freeSymbolTable(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != NULL) {
            Symbol* nextSym = sym->next;
            free(sym->name);
            if (sym->type == TYPE_INT && sym->size > 0) {
                free(sym->value.intArray);  // Free array if it exists
            } else if (sym->type == TYPE_FLOAT && sym->size > 0) {
                free(sym->value.floatArray);  // Free array if it exists
            }
            free(sym);  // Free the symbol
            sym = nextSym;
        }
    }
    free(table->table);
    free(table);
}

// Function to print the symbol table (for debugging)
void printSymbolTable(const SymbolTable* table) {
    printf("\n\n----- SYMBOL TABLE -----\n");
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != NULL) {
            if (sym->size > 0) {  // Check if it's an array
                printf("Name: %s | Type: %s | Array Size: %d\n", sym->name, symbolTypeToString(sym->type), sym->size);
                if (sym->type == TYPE_INT) {
                    for (int j = 0; j < sym->size; j++) {
                        printf("\tIndex %d: %d\n", j, sym->value.intArray[j]);
                    }
                } else if (sym->type == TYPE_FLOAT) {
                    for (int j = 0; j < sym->size; j++) {
                        printf("\tIndex %d: %.4f\n", j, sym->value.floatArray[j]);  // Format floats to 4 decimal places
                    }
                }
            } else {
                switch (sym->type) {
                    case TYPE_INT:
                        printf("Name: %s | Type: INT | Value: %d\n", sym->name, sym->value.intValue);
                        break;
                    case TYPE_FLOAT:
                        printf("Name: %s | Type: FLOAT | Value: %.4f\n", sym->name, sym->value.floatValue);  // Format float to 4 decimal places
                        break;
                    case TYPE_CHAR:
                        printf("Name: %s | Type: CHAR | Value: %c\n", sym->name, sym->value.charValue);
                        break;
                    case TYPE_UNKNOWN:
                    default:
                        printf("Name: %s | Type: UNKNOWN\n", sym->name);
                        break;
                }
            }
            sym = sym->next;
        }
    }
    printf("------------------------\n");
}


// Helper function to convert SymbolType to a string
const char* symbolTypeToString(SymbolType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_UNKNOWN: return "unknown";
        default: return "undefined";
    }
}
