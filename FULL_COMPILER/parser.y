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
        struct ASTNode* paramList; // Add paramList here
    } FunctionHeaderInfo;
}

// Include the helper function in parser.tab.c
%code {
    FunctionHeaderInfo* createFunctionHeader(const char* name) {
        FunctionHeaderInfo* header = (FunctionHeaderInfo*)malloc(sizeof(FunctionHeaderInfo));
        if (header == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for FunctionHeaderInfo.\n");
            exit(EXIT_FAILURE);
        }
        header->name = strdup(name);
        header->paramList = NULL; // Initially no parameters
        return header;
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
%token <character> SEMICOLON
%token <character> COMMA
%token <operator> EQ
%token <operator> PLUS
%token <operator> MINUS
%token <operator> MULTIPLY
%token <operator> DIVIDE
%token <operator> GT LT GE LE EQEQ NEQ
%token <integer> INTEGER
%token <floatNum> FLOAT
%token <string> WRITE
%token LPAREN RPAREN
%token LBRACKET RBRACKET
%token FUNCTION RETURN LBRACE RBRACE
%token IF ELSE
%token WHILE
%token UNRECOGNIZED

%nonassoc GT LT GE LE EQEQ NEQ
%left PLUS MINUS
%left MULTIPLY DIVIDE

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr ArrayAccess FunctionDef FunctionDefList
%type <funcHeader> FunctionHeader
%type <ast> Param ParamList ParamListTail ArgList ArgListTail
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

        // Add function to symbol table
        addFunctionSymbol(symTab, $1->name);

        // Merge paramList (if any) into varDeclList
        if ($1->paramList != NULL) {
            if ($2 == NULL) {
                $$->functionDef.varDeclList = $1->paramList;
            } else {
                // Append $2 at the end of $1->paramList
                ASTNode* paramTail = $1->paramList;
                while (paramTail->varDeclList.varDeclList != NULL) {
                    paramTail = paramTail->varDeclList.varDeclList;
                }
                paramTail->varDeclList.varDeclList = $2;
                $$->functionDef.varDeclList = $1->paramList;
            }
        }

        printf("[INFO] Function defined: %s\n", $1->name);

        // Clean up
        free($1->name);
        free($1);
    }
;

FunctionHeader:
    FUNCTION ID LPAREN ParamList RPAREN LBRACE {
        $$ = createFunctionHeader($2);

        // Set the global functionBeingParsed
        functionBeingParsed = $$->name;

        // Enter a new scope for the function
        enterScope();

        // Add parameters to symbol table
        ASTNode* paramNode = $4; 
        $$->paramList = $4; // Store paramList in FunctionHeaderInfo
        while (paramNode != NULL && paramNode->type == NodeType_VarDeclList) {
            ASTNode* singleParam = paramNode->varDeclList.varDecl;
            if (singleParam && singleParam->type == NodeType_VarDecl) {
                SymbolValue value;
                SymbolType st = TYPE_UNKNOWN;
                if (strcmp(singleParam->varDecl.varType, "int") == 0) {
                    st = TYPE_INT;
                    value.intValue = INT_MIN;
                } else if (strcmp(singleParam->varDecl.varType, "float") == 0) {
                    st = TYPE_FLOAT;
                    value.floatValue = FLT_MIN;
                }
                // Use addParameterSymbol instead of addSymbol
                addParameterSymbol(currentScope, singleParam->varDecl.varName, st, value);
            }
            paramNode = paramNode->varDeclList.varDeclList;
        }
    }
;


FunctionFooter:
    RBRACE {
        // Exit the function's scope
        exitScope();

        // Reset the global functionBeingParsed
        functionBeingParsed = NULL;
    }
;

ParamList:
    /* empty */ {
        $$ = NULL; // No params
    }
    | Param ParamListTail {
        if ($2 == NULL) {
            $$ = createNode(NodeType_VarDeclList);
            $$->varDeclList.varDecl = $1; 
            $$->varDeclList.varDeclList = NULL;
        } else {
            $$ = createNode(NodeType_VarDeclList);
            $$->varDeclList.varDecl = $1;
            $$->varDeclList.varDeclList = $2;
        }
    }
;

ParamListTail:
    /* empty */ { $$ = NULL; }
    | COMMA Param ParamListTail {
        ASTNode* newList = createNode(NodeType_VarDeclList);
        newList->varDeclList.varDecl = $2;
        newList->varDeclList.varDeclList = $3;
        $$ = newList;
    }
;

Param:
    TYPE ID {
        $$ = createNode(NodeType_VarDecl);
        $$->varDecl.varType = strdup($1);
        $$->varDecl.varName = strdup($2);
    }
;

ArgList:
    /* empty */ {
        $$ = NULL;
    }
    | Expr ArgListTail {
        // If there is just one expr
        if ($2 == NULL) {
            $$ = createNode(NodeType_StmtList); // Reuse StmtList or create a new node type for arguments
            $$->stmtList.stmt = $1;
            $$->stmtList.stmtList = NULL;
        } else {
            // $2 is another list, chain them
            $$ = createNode(NodeType_StmtList);
            $$->stmtList.stmt = $1;
            $$->stmtList.stmtList = $2;
        }
    }
;

ArgListTail:
    /* empty */ { $$ = NULL; }
    | COMMA Expr ArgListTail {
        ASTNode* newList = createNode(NodeType_StmtList);
        newList->stmtList.stmt = $2;
        newList->stmtList.stmtList = $3;
        $$ = newList;
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
            addSymbol(currentScope, $2, TYPE_INT, value);
        } else if (strcmp($1, "float") == 0) {
            value.floatValue = FLT_MIN; // Default value for uninitialized float
            addSymbol(currentScope, $2, TYPE_FLOAT, value);
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
            addArraySymbol(currentScope, $2, TYPE_INT, value, $4);  // $4 is the array size
        } else if (strcmp($1, "float") == 0) {
            value.floatArray = (float*)malloc($4 * sizeof(float));  // Allocate space for float array
            addArraySymbol(currentScope, $2, TYPE_FLOAT, value, $4);  // $4 is the array size
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
        Symbol* sym = lookupSymbol(currentScope, $1);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        printf("[DEBUG] Assignment detected: %s = Expr\n", $1); // Debug print

        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr($3, currentScope);  // Cast to int
            printf("[DEBUG] Assigning value to %s: %d\n", $1, value); // Debug print
            sym->value.intValue = value;
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr($3, currentScope);  // Handle float values
            printf("[DEBUG] Assigning value to %s: %f\n", $1, value); // Debug print
            sym->value.floatValue = value;
        }

        $$ = createNode(NodeType_AssignStmt);
        $$->assignStmt.varName = strdup($1);
        $$->assignStmt.operator = strdup($2);
        $$->assignStmt.expr = $3;
        $$->assignStmt.isArray = 0; // It's a normal variable
        $$->assignStmt.arrayIndex = NULL;

        printf("[INFO] Assignment statement recognized: %s = ...;\n", $1);
    }
    | ArrayAccess EQ Expr SEMICOLON {
        printf("[DEBUG] Array assignment detected: %s[index] = Expr\n", $1->arrayAccess.arrayName); // Debug print

        Symbol* sym = lookupSymbol(currentScope, $1->arrayAccess.arrayName);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared array '%s' used at line %d.\n", $1->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        int index = (int)evaluateExpr($1->arrayAccess.index, currentScope);  // Evaluate index expression
        printf("[DEBUG] Array index evaluated: %d\n", index); // Debug print for index value

        if (index < 0 || index >= sym->size) {
            fprintf(stderr, "Error: Array index out of bounds for array '%s' at line %d.\n", $1->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        // Evaluate the value to be assigned and store in the array based on its type
        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr($3, currentScope); // Evaluate and cast to int for int arrays
            printf("[DEBUG] Assigning int value to %s[%d]: %d\n", $1->arrayAccess.arrayName, index, value); // Debug print
            sym->value.intArray[index] = value;  // Store in the int array
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr($3, currentScope); // Evaluate as float for float arrays
            printf("[DEBUG] Assigning float value to %s[%d]: %f\n", $1->arrayAccess.arrayName, index, value); // Debug print
            sym->value.floatArray[index] = value;  // Store in the float array
        }

        // Generate TAC with the array name and index
        char arrayAccessStr[50];
        sprintf(arrayAccessStr, "%s[%d]", $1->arrayAccess.arrayName, index);  // Create string "arr[0]" format

        // $1 is <ast>, from the ArrayAccess rule above, so $1->arrayAccess.index is ASTNode*.
        $$ = createNode(NodeType_AssignStmt);
        $$->assignStmt.varName = strdup($1->arrayAccess.arrayName);
        $$->assignStmt.operator = strdup($2);
        $$->assignStmt.expr = $3;
        $$->assignStmt.isArray = 1;
        $$->assignStmt.arrayIndex = $1->arrayAccess.index; // $1->arrayAccess.index is ASTNode*, matches arrayIndex type.

        printf("[INFO] Array assignment recognized: %s[%d] = ...;\n", $1->arrayAccess.arrayName, index);
    }
    | WRITE Expr SEMICOLON {
        $$ = createNode(NodeType_WriteStmt);
        char* id = NULL;

        if ($2->type == NodeType_SimpleID) {
            Symbol* sym = lookupSymbol(currentScope, $2->simpleID.name);
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
        printf("[INFO] Write statement recognized: write %s;\n", id);
    }
    | ID LPAREN ArgList RPAREN SEMICOLON {
        Symbol* sym = lookupSymbol(currentScope, $1);
        if (sym == NULL || sym->type != TYPE_FUNCTION) {
            fprintf(stderr, "Error: Undeclared function '%s' called at line %d.\n", $1, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }
        $$ = createNode(NodeType_FunctionCall);
        $$->functionCall.name = strdup($1);
        $$->functionCall.args = $3;
        // This represents a function call as a statement (no return value used)
        printf("[INFO] Function call statement recognized: %s(...);\n", $1);
    }
    | IF LPAREN Expr RPAREN Stmt ELSE Stmt {
        $$ = createNode(NodeType_IfStmt);
        $$->ifStmt.condition = $3;
        $$->ifStmt.thenStmt = $5;
        $$->ifStmt.elseStmt = $7;
        printf("[INFO] If-Else statement recognized.\n");
    }
    | IF LPAREN Expr RPAREN Stmt {
        $$ = createNode(NodeType_IfStmt);
        $$->ifStmt.condition = $3;
        $$->ifStmt.thenStmt = $5;
        $$->ifStmt.elseStmt = NULL;
        printf("[INFO] If statement recognized.\n");
    }
    | LBRACE StmtList RBRACE {
        $$ = $2; // StmtList already represents the list of statements in the block
        printf("[INFO] Block statement recognized.\n");
    }
    | RETURN Expr SEMICOLON {
        $$ = createNode(NodeType_ReturnStmt);
        $$->returnStmt.expr = $2;
        printf("[INFO] Return statement recognized.\n");
    }
    | WHILE LPAREN Expr RPAREN LBRACE StmtList RBRACE {
        $$ = createNode(NodeType_WhileStmt);
        $$->whileStmt.condition = $3;
        $$->whileStmt.body = $6;

        printf("[INFO] While loop recognized.\n");
    }
;

ArrayAccess:
    ID LBRACKET Expr RBRACKET {
        // Lookup the array in the symbol table
        Symbol* sym = lookupSymbol(currentScope, $1);
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
    | Expr GT Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup(">");
        printf("[INFO] Expression recognized: ... > ...\n");
    }
    | Expr LT Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup("<");
        printf("[INFO] Expression recognized: ... < ...\n");
    }
    | Expr GE Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup(">=");
        printf("[INFO] Expression recognized: ... >= ...\n");
    }
    | Expr LE Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup("<=");
        printf("[INFO] Expression recognized: ... <= ...\n");
    }
    | Expr EQEQ Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup("==");
        printf("[INFO] Expression recognized: ... == ...\n");
    }
    | Expr NEQ Expr {
        $$ = createNode(NodeType_Expr);
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = strdup("!=");
        printf("[INFO] Expression recognized: ... != ...\n");
    }
    | ID {
        Symbol* sym = lookupSymbol(currentScope, $1);
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
    | ID LPAREN ArgList RPAREN {
        $$ = createNode(NodeType_FunctionCall);
        $$->functionCall.name = strdup($1);
        $$->functionCall.args = $3; // ArgList AST node
        printf("[INFO] Function call expression recognized: %s(...)\n", $1);
    }
;

%%

int main() {
    // Initialize the global symbol table (global scope)
    symTab = createSymbolTable(NULL, TABLE_SIZE);
    if (symTab == NULL || symTab->table == NULL) {
        fprintf(stderr, "[ERROR] Failed to initialize global symbol table.\n");
        return EXIT_FAILURE;
    }

    // Set the global scope as the current scope
    currentScope = symTab;
    registerScope(currentScope);

    if (scopeListHead == NULL) {
        fprintf(stderr, "[ERROR] Scope list head is NULL after registering global scope.\n");
        return EXIT_FAILURE;
    }

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

        printf("\n----- ABSTRACT SYNTAX TREE -----\n");
        if (root != NULL) {
            traverseAST(root, 0);
        }

        // Print the symbol table for all scopes
        printSymbolTable(scopeListHead);

        // Generate TAC
        printf("\n----- GENERATED TAC -----\n");
        TAC* tacHead = generateTAC(root, NULL);
        printTAC(tacHead);

        // --- (2) Generate MIPS directly from the AST + symbol tables ---
        printf("\n----- GENERATING MIPS CODE -----\n");
        // Then generate MIPS from the TAC
        generateMIPSFromTAC(tacHead, "output.s");

        printf("[INFO] MIPS code generated in output.s\n");

        // Cleanup TAC
        freeTAC(tacHead);

        // Clean up the AST
        if (root != NULL) {
            freeAST(root);
        }
    } else {
        fprintf(stderr, "Parsing failed with errors\n");
    }

    // Cleanup the symbol tables
    ScopeNode* currentNode = scopeListHead;
    while (currentNode != NULL) {
        if (currentNode->scope != NULL) {
            freeSymbolTable(currentNode->scope);
        }
        ScopeNode* tempNode = currentNode;
        currentNode = currentNode->next;
        free(tempNode);
    }
    scopeListHead = NULL;

    fclose(yyin);
    return parseResult;
}


void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    parseErrorFlag = 1;
}