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
int parseErrorFlag = 0;  // Flag to indicate if an error occurred during parsing
%}

%union {
    int integer;
    float floatNum;
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
%token <integer> INTEGER
%token <floatNum> FLOAT
%token <string> WRITE
%token LPAREN RPAREN
%token UNRECOGNIZED

%left PLUS MINUS
%left MULTIPLY DIVIDE

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr
%start Program

%%

Program:
    VarDeclList StmtList {
        $$ = createNode(NodeType_Program);
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

        if (sym->type == TYPE_INT) {
            sym->value.intValue = (int)evaluateExpr($3, symTab);  // Cast to int
            char* exprResult = generateExprTAC($3, symTab);
            generateTAC("MOV", sym->name, exprResult, NULL);
        } else if (sym->type == TYPE_FLOAT) {
            sym->value.floatValue = evaluateExpr($3, symTab);  // Handle float values
            char* exprResult = generateExprTAC($3, symTab);
            generateTAC("MOV", sym->name, exprResult, NULL);
        }

        $$ = createNode(NodeType_AssignStmt);
        $$->assignStmt.varName = strdup($1);
        $$->assignStmt.operator = strdup($2);
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

        $$ = createNode(NodeType_WriteStmt);
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
        printTAC();

        // Step 1: Clean the TAC by replacing variable references with temp vars
        printf("\n----- CLEANED TAC -----\n");
        replaceVariablesWithTemp(&tacHead);
        printTAC();

        // Step 2: Optimize the cleaned-up TAC
        optimizeTAC(&tacHead);

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
