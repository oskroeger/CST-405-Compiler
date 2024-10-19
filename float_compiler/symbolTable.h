#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Define the Symbol struct
typedef struct Symbol {
    char* name;
    char* type;
    int intValue;
    float floatValue;
    struct Symbol* next;  // For handling collisions in the hash table
} Symbol;

// Define the SymbolTable struct
typedef struct SymbolTable {
    int size;
    struct Symbol** table;  // Array of pointers to Symbols
} SymbolTable;

// Create a new symbol table with a given size
SymbolTable* createSymbolTable(int size);

// Add a new symbol to the symbol table
void addSymbol(SymbolTable* table, char* name, char* type, float value);

// Look up a symbol by name in the symbol table
Symbol* lookupSymbol(SymbolTable* table, char* name);

// Free all memory associated with the symbol table
void freeSymbolTable(SymbolTable* table);

// Print the contents of the symbol table (for debugging)
void printSymbolTable(const SymbolTable* table);

#endif // SYMBOL_TABLE_H