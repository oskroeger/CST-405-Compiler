%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include "AST.h"
#include "symbolTable.h"
#include "semantic.h"
#include "codeGenerator.h"
#include "optimizer.h"

#define TABLE_SIZE 100

extern int yylex();        // Declare yylex, the lexer function
extern int yyparse(void);  // Declare yyparse, the parser function
extern FILE* yyin;         // Declare yyin, the file pointer for the input file
extern int yylineno;       // Declare yylineno, the line number counter from Bison
extern TAC* tacHead;       // Declare the head of the linked list of TAC entries
extern struct ASTNode* root;

void yyerror(const char* s);

ASTNode* root = NULL;
SymbolTable* symTab = NULL;  // Initialize the symbol table pointer
Symbol* symbol = NULL;
int parseErrorFlag = 0;      // Flag to indicate if an error occurred during parsing

// Global variable to hold the current function name
char* functionBeingParsed = NULL;

// You can keep function prototypes and helper functions here

%}

// Include the definition of FunctionHeaderInfo in parser.tab.h
%code requires {
    typedef struct {
        char* name;
        char** paramNames;
        char** paramTypes;
        int paramCount;
    } FunctionHeaderInfo;
}

// Include the helper function in parser.tab.c
%code {
    FunctionHeaderInfo* createFunctionHeader(const char* name, int paramCount) {
        FunctionHeaderInfo* header = (FunctionHeaderInfo*)malloc(sizeof(FunctionHeaderInfo));
        if (header == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for FunctionHeaderInfo.\n");
            exit(EXIT_FAILURE);
        }
        header->name = strdup(name);
        header->paramCount = paramCount;
        header->paramNames = (char**)malloc(paramCount * sizeof(char*));
        header->paramTypes = (char**)malloc(paramCount * sizeof(char*));
        return header;
    }

    void freeFunctionHeader(FunctionHeaderInfo* header) {
        if (header) {
            free(header->name);
            for (int i = 0; i < header->paramCount; i++) {
                free(header->paramNames[i]);
                free(header->paramTypes[i]);
            }
            free(header->paramNames);
            free(header->paramTypes);
            free(header);
        }
    }
}

%union {
    int integer;
    float floatNum;
    char character;
    char* string;
    char* operator;
    struct ASTNode* ast;
    FunctionHeaderInfo* funcHeader;
}

%token <string> TYPE
%token <string> ID
%token <char> SEMICOLON
%token <operator> EQ
%token <operator> PLUS
%token <operator> MINUS
%token <operator> MULTIPLY
%token <operator> DIVIDE
%token <integer> INTEGER
%token <floatNum> FLOAT
%token <string> WRITE
%token LPAREN RPAREN
%token LBRACKET RBRACKET
%token FUNCTION RETURN LBRACE RBRACE
%token UNRECOGNIZED

%left PLUS MINUS
%left MULTIPLY DIVIDE

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr ArrayAccess FunctionDef FunctionDefList
%type <funcHeader> FunctionHeader
%start Program

%%

Program:
    FunctionDefList VarDeclList StmtList {
        $$ = createNode(NodeType_Program);
        $$->program.functionDefList = $1;
        $$->program.varDeclList = $2;
        $$->program.stmtList = $3;
        root = $$; // Set the global root pointer to the root of the AST

        printf("\n[INFO] Program parsed successfully with functions.\n");
    }
;

FunctionDefList:
    /* empty */ {
        $$ = NULL; // No functions defined
    }
    | FunctionDef FunctionDefList {
        $$ = createNode(NodeType_FunctionDefList);
        $$->functionDefList.functionDef = $1;
        $$->functionDefList.functionDefList = $2;

        printf("[INFO] Function definition list recognized.\n");
    }
;

FunctionDef:
    FunctionHeader VarDeclList StmtList FunctionFooter {
        $$ = createNode(NodeType_FunctionDef);
        $$->functionDef.name = strdup($1->name);
        $$->functionDef.varDeclList = $2;
        $$->functionDef.stmtList = $3;

        // Insert function parameters into symbol table
        for (int i = 0; i < $1->paramCount; i++) {
            SymbolValue val;
            addSymbol(symTab, $1->paramNames[i], getTypeFromString($1->paramTypes[i]), val);
        }

        // Add function to symbol table
        addFunctionSymbol(symTab, $1->name);

        printf("[INFO] Function defined: %s\n", $1->name);

        // Clean up
        freeFunctionHeader($1);
    }
;

FunctionHeader:
    FUNCTION ID LPAREN ParamList RPAREN LBRACE {
        $$ = createFunctionHeader($2, $3.paramCount);
        for (int i = 0; i < $3.paramCount; i++) {
            $$->paramNames[i] = strdup($3.paramNames[i]);
            $$->paramTypes[i] = strdup($3.paramTypes[i]);
        }

        functionBeingParsed = $$->name;
        generateTAC("FUNC_START", functionBeingParsed, NULL, NULL);
    }
;

ParamList:
    /* empty */ {
        $$ = (ParamListData){0, NULL, NULL};
    }
    | ParamListWithTypes {
        $$ = $1;
    }
;

ParamListWithTypes:
    TYPE ID {
        $$ = (ParamListData){1, malloc(sizeof(char*)), malloc(sizeof(char*))};
        $$->paramNames[0] = strdup($2);
        $$->paramTypes[0] = strdup($1);
    }
    | ParamListWithTypes COMMA TYPE ID {
        $$ = $1;
        $$->paramNames = realloc($$->paramNames, ($$->paramCount + 1) * sizeof(char*));
        $$->paramTypes = realloc($$->paramTypes, ($$->paramCount + 1) * sizeof(char*));
        $$->paramNames[$$->paramCount] = strdup($4);
        $$->paramTypes[$$->paramCount] = strdup($3);
        $$->paramCount++;
    }
;

FunctionFooter:
    RBRACE {
        // Generate TAC for function end
        generateTAC("FUNC_END", functionBeingParsed, NULL, NULL);

        // Reset the global functionBeingParsed
        functionBeingParsed = NULL;
    }
;

VarDeclList:
    /* empty */ {
        $$ = NULL; // Handle case where there are no variable declarations
    }
    | VarDecl VarDeclList {
        $$ = createNode(NodeType_VarDeclList);
        $$->varDeclList.varDecl = $1;
        $$->varDeclList.varDeclList = $2;

        printf("[INFO] Variable declaration list recognized.\n");
    }
;

VarDecl:
    TYPE ID SEMICOLON {
        SymbolValue value;
        if (strcmp($1, "int") == 0) {
            value.intValue = INT_MIN; // Default value for uninitialized int
            addSymbol(symTab, $2, TYPE_INT, value);
        } else if (strcmp($1, "float") == 0) {
            value.floatValue = FLT_MIN; // Default value for uninitialized float
            addSymbol(symTab, $2, TYPE_FLOAT, value);
        }

        $$ = createNode(NodeType_VarDecl);
        $$->varDecl.varType = strdup($1); // Set the variable type (e.g., "int", "float")
        $$->varDecl.varName = strdup($2); // Set the variable name

        printf("[INFO] Variable declared: %s %s;\n", $1, $2);
    }
    | TYPE ID LBRACKET INTEGER RBRACKET SEMICOLON {
        SymbolValue value;
        if (strcmp($1, "int") == 0) {
            value.intArray = (int*)malloc($4 * sizeof(int));  // Allocate space for int array
            addArraySymbol(symTab, $2, TYPE_INT, value, $4);  // $4 is the array size
        } else if (strcmp($1, "float") == 0) {
            value.floatArray = (float*)malloc($4 * sizeof(float));  // Allocate space for float array
            addArraySymbol(symTab, $2, TYPE_FLOAT, value, $4);  // $4 is the array size
        }

        $$ = createNode(NodeType_ArrayDecl);
        $$->arrayDecl.varType = strdup($1);  // Set the variable type (e.g., "int", "float")
        $$->arrayDecl.varName = strdup($2);  // Set the variable name
        $$->arrayDecl.size = $4;             // Set the array size

        printf("[INFO] Array declared: %s %s[%d];\n", $1, $2, $4);
    }
;

StmtList:
    /* empty */ {
        $$ = NULL; // Handle the case where there are no statements
    }
    | Stmt StmtList {
        $$ = createNode(NodeType_StmtList);
        $$->stmtList.stmt = $1;
        $$->stmtList.stmtList = $2;

        printf("[INFO] Statement list recognized.\n");
    }
;

Stmt:
    ID EQ Expr SEMICOLON {
        Symbol* sym = lookupSymbol(symTab, $1);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        printf("[DEBUG] Assignment detected: %s = Expr\n", $1); // Debug print

        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr($3, symTab);  // Cast to int
            printf("[DEBUG] Assigning value to %s: %d\n", $1, value); // Debug print
            sym->value.intValue = value;
            char* exprResult = generateExprTAC($3, symTab);
            generateTAC("MOV", sym->name, exprResult, NULL);
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr($3, symTab);  // Handle float values
            printf("[DEBUG] Assigning value to %s: %f\n", $1, value); // Debug print
            sym->value.floatValue = value;
            char* exprResult = generateExprTAC($3, symTab);
            generateTAC("MOV", sym->name, exprResult, NULL);
        }

        $$ = createNode(NodeType_AssignStmt);
        $$->assignStmt.varName = strdup($1);
        $$->assignStmt.operator = strdup($2);
        $$->assignStmt.expr = $3;

        printf("[INFO] Assignment statement recognized: %s = ...;\n", $1);
    }
    | ArrayAccess EQ Expr SEMICOLON {
        printf("[DEBUG] Array assignment detected: %s[index] = Expr\n", $1->arrayAccess.arrayName); // Debug print

        Symbol* sym = lookupSymbol(symTab, $1->arrayAccess.arrayName);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared array '%s' used at line %d.\n", $1->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        int index = (int)evaluateExpr($1->arrayAccess.index, symTab);  // Evaluate index expression
        printf("[DEBUG] Array index evaluated: %d\n", index); // Debug print for index value

        if (index < 0 || index >= sym->size) {
            fprintf(stderr, "Error: Array index out of bounds for array '%s' at line %d.\n", $1->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        // Evaluate the value to be assigned and store in the array based on its type
        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr($3, symTab); // Evaluate and cast to int for int arrays
            printf("[DEBUG] Assigning int value to %s[%d]: %d\n", $1->arrayAccess.arrayName, index, value); // Debug print
            sym->value.intArray[index] = value;  // Store in the int array
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr($3, symTab); // Evaluate as float for float arrays
            printf("[DEBUG] Assigning float value to %s[%d]: %f\n", $1->arrayAccess.arrayName, index, value); // Debug print
            sym->value.floatArray[index] = value;  // Store in the float array
        }

        // Generate TAC for the array assignment
        char* exprResult = generateExprTAC($3, symTab);

        // Generate TAC with the array name and index
        char arrayAccessStr[50];
        sprintf(arrayAccessStr, "%s[%d]", $1->arrayAccess.arrayName, index);  // Create string "arr[0]" format

        // Use the array access string directly for TAC generation
        generateTAC("MOV", arrayAccessStr, exprResult, NULL);

        $$ = createNode(NodeType_AssignStmt);
        $$->assignStmt.varName = strdup($1->arrayAccess.arrayName);
        $$->assignStmt.operator = strdup($2);
        $$->assignStmt.expr = $3;

        printf("[INFO] Array assignment recognized: %s[%d] = ...;\n", $1->arrayAccess.arrayName, index);
    }
    | WRITE Expr SEMICOLON {
        $$ = createNode(NodeType_WriteStmt);
        char* id = NULL;

        if ($2->type == NodeType_SimpleID) {
            Symbol* sym = lookupSymbol(symTab, $2->simpleID.name);
            if (sym == NULL) {
                fprintf(stderr, "Error: Undeclared variable '%s' used in write statement at line %d.\n", $2->simpleID.name, yylineno);
                parseErrorFlag = 1;
                YYABORT;
            }
            id = strdup($2->simpleID.name);
        } else if ($2->type == NodeType_IntExpr) {
            id = strdup("intExpr");
        }

        printf("[DEBUG] Write statement detected: write %s\n", id); // Debug print

        $$->writeStmt.id = id;
        generateTAC("WRITE", id, NULL, NULL);
        printf("[INFO] Write statement recognized: write %s;\n", id);
    }
    | ID LPAREN RPAREN SEMICOLON {
        // Function call statement
        Symbol* sym = lookupSymbol(symTab, $1);
        if (sym == NULL || sym->type != TYPE_FUNCTION) {
            fprintf(stderr, "Error: Undeclared function '%s' called at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        $$ = createNode(NodeType_FunctionCall);
        $$->functionCall.name = strdup($1);

        printf("[INFO] Function call recognized: %s();\n", $1);

        // Generate TAC for function call
        generateTAC("CALL", $1, NULL, NULL);
    }
;

ArrayAccess:
    ID LBRACKET Expr RBRACKET {
        // Lookup the array in the symbol table
        Symbol* sym = lookupSymbol(symTab, $1);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared array '%s' used at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        $$ = createNode(NodeType_ArrayAccess);
        $$->arrayAccess.arrayName = strdup($1);   // Store the array name
        $$->arrayAccess.index = $3;               // Store the index expression

        printf("[INFO] Array access recognized: %s[index]\n", $1);
    }
;

Expr:
    Expr PLUS Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup($2);

        printf("[INFO] Expression recognized: ... + ...\n");
    }
    | Expr MINUS Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup($2);

        printf("[INFO] Expression recognized: ... - ...\n");
    }
    | Expr MULTIPLY Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup($2);

        printf("[INFO] Expression recognized: ... * ...\n");
    }
    | Expr DIVIDE Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup($2);

        printf("[INFO] Expression recognized: ... / ...\n");
    }
    | LPAREN Expr RPAREN {
        $$ = $2;
        printf("[INFO] Parenthesized expression recognized: (...)\n");
    }
    | ID {
        Symbol* sym = lookupSymbol(symTab, $1);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        $$ = createNode(NodeType_SimpleID);
        $$->simpleID.name = strdup($1);

        printf("[INFO] Identifier recognized: %s\n", $1);
    }
    | INTEGER {
        $$ = createNode(NodeType_IntExpr);
        $$->IntExpr.integer = $1;

        printf("[INFO] Integer recognized: %d\n", $1);
    }
    | FLOAT {
        $$ = createNode(NodeType_FloatExpr);
        $$->FloatExpr.floatNum = $1;

        printf("[INFO] FLOAT recognized: %f\n", $1);
    }
    | ArrayAccess {
        $$ = $1;
    }
;

%%

int main() {
    // Initialize the symbol table
    symTab = createSymbolTable(TABLE_SIZE);

    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");
    if (!yyin) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    printf("\n\n------- START COMPILER -------\n\n");

    int parseResult = yyparse();
    if (parseResult == 0) {
        printf("\n\n--- Parsing successful! ---\n\n");

        // Perform semantic analysis
        // checkSemantics(root, symTab);

        printf("\n----- ABSTRACT SYNTAX TREE -----\n");
        if (root != NULL) {
            traverseAST(root, 0);
        }

        printSymbolTable(symTab);

        // After TAC generation
        printf("\n----- GENERATED TAC (BEFORE OPTIMIZATION) -----\n");
        // printTAC();

        // Step 1: Clean the TAC by replacing variable references with temp vars
        printf("\n----- CLEANED TAC -----\n");
        // replaceVariablesWithTemp(&tacHead, symTab);
        // printTAC();

        // Step 2: Optimize the cleaned-up TAC
        // optimizeTAC(&tacHead);

        // Generate MIPS code from the TAC
        printf("\n----- GENERATED MIPS CODE -----\n");
        // generateMIPS(tacHead);

        // Clean up the AST
        if (root != NULL) {
            freeAST(root);
        }
    } else {
        fprintf(stderr, "Parsing failed with errors\n");
    }

    // Cleanup symbol table
    freeSymbolTable(symTab);

    fclose(yyin);
    return parseResult;
}

void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    parseErrorFlag = 1;
}
