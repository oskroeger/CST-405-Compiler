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
    TYPE_FUNCTION,
    TYPE_UNKNOWN  // Add TYPE_UNKNOWN as a fallback for unrecognized types
} SymbolType;

// Define a union for storing variable values
typedef union {
    int intValue;
    float floatValue;
    char charValue;
    char* stringValue;  // Add a string value for char arrays
    int* intArray;      // Array of integers
    float* floatArray;  // Array of floats
} SymbolValue;

// Define the Symbol struct
typedef struct Symbol {
    char* name;
    SymbolType type;
    SymbolValue value;
    int size;           // Array size (0 for non-array variables)
    struct Symbol* next;  // For handling collisions in the hash table

    // Additional fields for functions
    char* returnType;          // Return type for functions (NULL if it's not a function)
    struct ParamList* params;  // Linked list of parameters for function symbols (NULL if not a function)
} Symbol;

// Define the SymbolTable struct
typedef struct SymbolTable {
    int size;
    struct Symbol** table;  // Array of pointers to Symbols
} SymbolTable;

// Function prototypes for the symbol table
SymbolTable* createSymbolTable(int size);
void addSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value);
void addArraySymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value, int size);  // Add arrays
void addFunctionSymbol(SymbolTable* table, char* name, char* returnType, char* paramList) ;
Symbol* lookupSymbol(SymbolTable* table, char* name);
void freeSymbolTable(SymbolTable* table);
void printSymbolTable(const SymbolTable* table);

// Helper function to convert SymbolType to a string
const char* symbolTypeToString(SymbolType type);

#endif // SYMBOL_TABLE_H