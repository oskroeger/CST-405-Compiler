%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int parseErrorFlag = 0;  // Flag to indicate if an error occurred during parsing
%}

%union {
    int number;
    char character;
    char* string;
    char* operator;
    struct ASTNode* ast;
}

%token <string> TYPE
%token <string> ID
%token <char> SEMICOLON
%token <operator> EQ
%token <operator> PLUS
%token <operator> MINUS
%token <operator> MULTIPLY
%token <operator> DIVIDE
%token <number> NUMBER
%token <string> WRITE
%token LPAREN RPAREN
%token UNRECOGNIZED

%left PLUS MINUS
%left MULTIPLY DIVIDE

%printer { fprintf(yyoutput, "%s", $$); } ID;

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr
%start Program

%%

Program:
    VarDeclList StmtList {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for Program node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_Program;
        $$->program.varDeclList = $1;
        $$->program.stmtList = $2;
        root = $$; // Set the global root pointer to the root of the AST

        printf("\n[INFO] Program parsed successfully.\n");
    }
;

VarDeclList:
    /* empty */ {
        $$ = NULL; // Handle case where there are no variable declarations
    }
    | VarDecl VarDeclList {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for VarDeclList node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_VarDeclList;
        $$->varDeclList.varDecl = $1;
        $$->varDeclList.varDeclList = $2;

        printf("[INFO] Variable declaration list recognized.\n");
    }
;

VarDecl:
    TYPE ID SEMICOLON {
        if (lookupSymbol(symTab, $2) != NULL) {
            fprintf(stderr, "Error: Variable '%s' already declared at line %d.\n", $2, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for VarDecl node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_VarDecl;
        $$->varDecl.varType = strdup($1);
        $$->varDecl.varName = strdup($2);
        addSymbol(symTab, $2, $1, INT_MIN);

        printf("[INFO] Variable declared: %s %s;\n", $1, $2);
    }
    | TYPE ID /* missing semicolon */ {
        fprintf(stderr, "Error: Missing semicolon after variable declaration at line %d.\n", yylineno - 1);
        parseErrorFlag = 1;
        YYABORT;
    }
;

StmtList:
    /* empty */ {
        $$ = NULL; // Handle the case where there are no statements
    }
    | Stmt StmtList {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for StmtList node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_StmtList;
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

        // Store the expression result in the symbol table
        sym->intValue = evaluateExpr($3, symTab);
        char* exprResult = generateExprTAC($3, symTab);
        generateTAC("MOV", sym->name, exprResult, NULL);

        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for AssignStmt node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_AssignStmt;
        $$->assignStmt.varName = strdup($1);
        $$->assignStmt.operator = $2;
        $$->assignStmt.expr = $3;

        printf("[INFO] Assignment statement recognized: %s = ...;\n", $1);
    }
    | ID EQ Expr /* missing semicolon */ {
        fprintf(stderr, "Error: Missing semicolon after assignment statement at line %d.\n", yylineno - 1);
        parseErrorFlag = 1;
        YYABORT;
    }
    | WRITE ID SEMICOLON {
        Symbol* sym = lookupSymbol(symTab, $2);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used in write statement at line %d.\n", $2, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for WriteStmt node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_WriteStmt;
        $$->writeStmt.id = strdup($2);

        generateTAC("WRITE", $$->writeStmt.id, NULL, NULL);
        printf("[INFO] Write statement recognized: write %s;\n", $2);
    }
    | WRITE ID /* missing semicolon */ {
        fprintf(stderr, "Error: Missing semicolon after write statement at line %d.\n", yylineno - 1);
        parseErrorFlag = 1;
        YYABORT;
    }
    | UNRECOGNIZED {
        fprintf(stderr, "Error: Unrecognized keyword or statement at line %d.\n", yylineno);
        parseErrorFlag = 1;
        YYABORT;
    }
;

Expr:
    Expr PLUS Expr {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for Expr node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_Expr;
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = $2;

        printf("[INFO] Expression recognized: ... + ...\n");
    }
    | Expr MINUS Expr {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for Expr node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_Expr;
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = $2;

        printf("[INFO] Expression recognized: ... - ...\n");
    }
    | Expr MULTIPLY Expr {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for Expr node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_Expr;
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = $2;

        printf("[INFO] Expression recognized: ... * ...\n");
    }
    | Expr DIVIDE Expr {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for Expr node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_Expr;
        $$->expr.left = $1;
        $$->expr.right = $3;
        $$->expr.operator = $2;

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

        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for SimpleID node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_SimpleID;
        $$->simpleID.name = $1;

        printf("[INFO] Identifier recognized: %s\n", $1);
    }
    | NUMBER {
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed for SimpleExpr node.\n");
            exit(EXIT_FAILURE);
        }
        $$->type = NodeType_SimpleExpr;
        $$->simpleExpr.number = $1;

        printf("[INFO] Number recognized: %d\n", $1);
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
        checkSemantics(root, symTab);

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
        // replaceVariablesWithTemp(&tacHead);
        // printTAC();

        // Step 2: Optimize the cleaned-up TAC
        // optimizeTAC(&tacHead);

        // Print the optimized TAC
        printf("\n----- OPTIMIZED TAC -----\n");
		// printOptimizedTAC();

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