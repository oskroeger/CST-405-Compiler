#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Enum for variable types
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_FUNCTION, // Added for functions
    TYPE_UNKNOWN    // Fallback for unrecognized types
} SymbolType;

// Define a union for storing variable values
typedef union {
    int intValue;
    float floatValue;
    char charValue;
    int* intArray;      // Array of integers
    float* floatArray;  // Array of floats
} SymbolValue;

// Define the Symbol struct
typedef struct Symbol {
    char* name;
    SymbolType type;
    SymbolValue value;
    int size;           // Array size (0 for non-array variables)
    char** paramNames;  // Parameter names for functions
    SymbolType* paramTypes; // Parameter types for functions
    int paramCount;     // Number of parameters for functions
    struct Symbol* next;  // For handling collisions in the hash table
} Symbol;

// Define the SymbolTable struct
typedef struct SymbolTable {
    int size;
    struct Symbol** table;  // Array of pointers to Symbols
} SymbolTable;

// Function prototypes for the symbol table
SymbolTable* createSymbolTable(int size);
void addSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value);
void addArraySymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value, int size);
void addFunctionSymbol(SymbolTable* table, char* name, char** paramNames, SymbolType* paramTypes, int paramCount); // Updated for functions with parameters
Symbol* lookupSymbol(SymbolTable* table, char* name);
void freeSymbolTable(SymbolTable* table);
void printSymbolTable(const SymbolTable* table);

// Helper function to convert SymbolType to a string
const char* symbolTypeToString(SymbolType type);

#endif // SYMBOL_TABLE_H
