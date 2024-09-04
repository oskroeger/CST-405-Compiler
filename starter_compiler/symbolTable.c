#include "symbolTable.h"
#include <stdlib.h>
#include <stddef.h> // Include the header file for NULL macro
#include <string.h>
#include <stdio.h>



// Function to create a new symbol table
SymbolTable* createSymbolTable(int size) {
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!newTable) return 0;

    newTable->size = size;
    newTable->table = (Symbol**)malloc(sizeof(Symbol*) * size);
    if (!newTable->table) {
        free(newTable);
        return 0;
    }

    for (int i = 0; i < size; i++) {
        newTable->table[i] = 0;
    }

    return newTable;
}

// Hash function to map a name to an index
unsigned int hash(SymbolTable* table, char* name) {
    unsigned int hashval = 0;
    for (; *name != '\0'; name++) hashval = *name + (hashval << 5) - hashval;
    return hashval % table->size;
}

// Function to add a symbol to the table
void addSymbol(SymbolTable* table, char* name, char* type, int value) {
    if (table == NULL || table->table == NULL) {
        fprintf(stderr, "Symbol table or table array not initialized\n");
        return; // Early return if the table is not initialized
    }

    unsigned int hashval = hash(table, name);
    // Check for an existing symbol with the same name
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            // Found an existing symbol, update its type
            free(sym->type); // Free the old type string
            sym->type = strdup(type); // Set new type
            sym->intValue = value; // Update value
            return; // Exit after updating
        }
    }

    // If no existing symbol found, add a new symbol
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) return;
    newSymbol->name = strdup(name);
    if (!newSymbol->name) {
        free(newSymbol);
        return;
    }
    newSymbol->type = strdup(type);
    if (!newSymbol->type) {
        free(newSymbol->name);
        free(newSymbol);
        return;
    }
    newSymbol->intValue = value;
    newSymbol->next = table->table[hashval];
    table->table[hashval] = newSymbol;
}

// Function to look up a name in the table
Symbol* lookupSymbol(SymbolTable* table, char* name) {
    printf("Looking up %s\n", name);
    unsigned int hashval = hash(table, name);

    // Search the linked list at table->table[hashval]
    // Check if the entry at hashval is null
    if (table->table[hashval] == NULL) {
        printf("No symbol found at hash value %u\n", hashval);
        return NULL;
    }
    
    printf("Symbol found at hash value %u\n", hashval);
    for (Symbol* sym = table->table[hashval]; sym != NULL; sym = sym->next) {
        printf("Symbol name: %s\n", sym->name);
        if (strcmp(name, sym->name) == 0) return sym;
    }
    
    return NULL;
}

// Function to free the symbol table
void freeSymbolTable(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != 0) {
            Symbol* nextSym = sym->next;
            free(sym->name);
            free(sym->type);
            // Free other dynamically allocated fields of Symbol
            free(sym);
            sym = nextSym;
        }
    }
    free(table->table);
    free(table);
}

// Function to print the symbol table
void printSymbolTable(const SymbolTable* table) {
    printf("\n\n----- SYMBOL TABLE -----\n");
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != 0) {
            if (sym->intValue == INT_MIN) {
                char val[9] = "Undefined";
                printf("Name: %s | Type: %s | Value: %s\n", sym->name, sym->type, val);
            }
            else
                printf("Name: %s | Type: %s | Value: %d\n", sym->name, sym->type, sym->intValue);
            // Print other fields of Symbol
            sym = sym->next;
        }
    }
    printf("------------------------\n");
}
