#include "symbolTable.h"
#include <stdlib.h>
#include <stddef.h>
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
            sym->type = type;
            if (type == TYPE_INT) {
                sym->value.intValue = value.intValue;
            } else if (type == TYPE_FLOAT) {
                sym->value.floatValue = value.floatValue;
            } else if (type == TYPE_CHAR) {
                sym->value.charValue = value.charValue;
            }
            return;
        }
    }

    // Add a new symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->size = 0;
    newSymbol->paramNames = NULL;
    newSymbol->paramTypes = NULL;
    newSymbol->paramCount = 0;

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

    // Add a new array symbol
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

    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

// Function to add a function symbol with parameters to the table
void addFunctionSymbol(SymbolTable* table, char* name, char** paramNames, SymbolType* paramTypes, int paramCount) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return;
    }

    unsigned int hashval = hash(table, name);

    // Check for an existing symbol with the same name
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            sym->type = TYPE_FUNCTION;
            return;
        }
    }

    // Add a new function symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;

    newSymbol->name = strdup(name);
    newSymbol->type = TYPE_FUNCTION;
    newSymbol->size = 0;
    newSymbol->paramNames = (char**)malloc(paramCount * sizeof(char*));
    newSymbol->paramTypes = (SymbolType*)malloc(paramCount * sizeof(SymbolType));
    newSymbol->paramCount = paramCount;

    for (int i = 0; i < paramCount; i++) {
        newSymbol->paramNames[i] = strdup(paramNames[i]);
        newSymbol->paramTypes[i] = paramTypes[i];
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
            if ((sym->type == TYPE_INT || sym->type == TYPE_FLOAT) && sym->size > 0) {
                if (sym->type == TYPE_INT) {
                    free(sym->value.intArray);
                } else if (sym->type == TYPE_FLOAT) {
                    free(sym->value.floatArray);
                }
            }
            if (sym->type == TYPE_FUNCTION) {
                for (int j = 0; j < sym->paramCount; j++) {
                    free(sym->paramNames[j]);
                }
                free(sym->paramNames);
                free(sym->paramTypes);
            }
            free(sym);
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
            if (sym->type == TYPE_FUNCTION) {
                printf("Name: %s | Type: FUNCTION | Parameters: ", sym->name);
                for (int j = 0; j < sym->paramCount; j++) {
                    printf("%s:%s", sym->paramNames[j], symbolTypeToString(sym->paramTypes[j]));
                    if (j < sym->paramCount - 1) printf(", ");
                }
                printf("\n");
            } else if (sym->size > 0) {
                printf("Name: %s | Type: %s | Array Size: %d\n", sym->name, symbolTypeToString(sym->type), sym->size);
                if (sym->type == TYPE_INT) {
                    for (int j = 0; j < sym->size; j++) {
                        printf("\tIndex %d: %d\n", j, sym->value.intArray[j]);
                    }
                } else if (sym->type == TYPE_FLOAT) {
                    for (int j = 0; j < sym->size; j++) {
                        printf("\tIndex %d: %.4f\n", j, sym->value.floatArray[j]);
                    }
                }
            } else {
                switch (sym->type) {
                    case TYPE_INT:
                        printf("Name: %s | Type: INT | Value: %d\n", sym->name, sym->value.intValue);
                        break;
                    case TYPE_FLOAT:
                        printf("Name: %s | Type: FLOAT | Value: %.4f\n", sym->name, sym->value.floatValue);
                        break;
                    case TYPE_CHAR:
                        printf("Name: %s | Type: CHAR | Value: %c\n", sym->name, sym->value.charValue);
                        break;
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
        case TYPE_FUNCTION: return "function";
        default: return "unknown";
    }
}
