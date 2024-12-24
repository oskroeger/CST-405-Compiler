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
    TYPE_UNKNOWN
} SymbolType;

// Define a union for storing variable values
typedef union {
    int intValue;
    float floatValue;
    char charValue;
    int* intArray;
    float* floatArray;
} SymbolValue;

// Define the Symbol struct
typedef struct Symbol {
    char* name;
    SymbolType type;
    SymbolValue value;
    int size;
    struct Symbol* next;
} Symbol;

// Forward declaration of SymbolTable
typedef struct SymbolTable SymbolTable;

// Define the SymbolTable struct with support for nested scopes
struct SymbolTable {
    int size;
    Symbol** table;       // Array of pointers to Symbols
    SymbolTable* parent;  // Pointer to the parent scope (NULL for global scope)
};

// Define the ScopeNode struct for tracking all scopes
typedef struct ScopeNode {
    SymbolTable* scope;
    struct ScopeNode* next;
} ScopeNode;

// Global variable to track the current scope
extern SymbolTable* currentScope;
extern ScopeNode* scopeListHead;

// Function prototypes for symbol table creation and manipulation
SymbolTable* createSymbolTable(SymbolTable* parent, int size);
void freeSymbolTable(SymbolTable* table);

void registerScope(SymbolTable* scope);

// Scope management functions
void enterScope();       // Creates a new scope
void exitScope();        // Exits the current scope

// Symbol management functions
void addSymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value);
void addArraySymbol(SymbolTable* table, char* name, SymbolType type, SymbolValue value, int size);
void addFunctionSymbol(SymbolTable* table, char* name);
Symbol* lookupSymbol(SymbolTable* table, char* name);
void printSymbolTable(const ScopeNode* scopeListHead);

// Helper function to convert SymbolType to a string
const char* symbolTypeToString(SymbolType type);

#endif // SYMBOL_TABLE_H