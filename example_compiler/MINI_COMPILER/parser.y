%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AST.h"
#include "symbolTable.h"
#include "semantic.h"
#include "codeGenerator.h"
#include "optimizer.h"

#define TABLE_SIZE 100

extern int yylex();   // Declare yylex, the lexer function
extern int yyparse(); // Declare yyparse, the parser function
extern FILE* yyin;    // Declare yyin, the file pointer for the input file
extern int yylineno;  // Declare yylineno, the line number counter
extern TAC* tacHead;  // Declare the head of the linked list of TAC entries

void yyerror(const char* s);

ASTNode* root = NULL; 
SymbolTable* symTab = NULL;
Symbol* symbol = NULL;

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
%token <number> NUMBER
%token <string> WRITE

%printer { fprintf(yyoutput, "%s", $$); } ID;

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr BinOp
%start Program

%%

Program: VarDeclList StmtList    { printf("The PARSER has started\n"); 
									root = malloc(sizeof(ASTNode));
									root->type = NodeType_Program;
									root->program.varDeclList = $1;
									root->program.stmtList = $2;
									// Set other fields as necessary
								}

;

VarDeclList:  {/*empty, i.e. it is possible not to declare a variable*/}
	| VarDecl VarDeclList {  printf("PARSER: Recognized variable declaration list\n"); 

							// Create AST node for VarDeclList
							$$ = malloc(sizeof(ASTNode));
							$$->type = NodeType_VarDeclList;
							$$->varDeclList.varDecl = $1;
							$$->varDeclList.varDeclList = $2;
				
							// Set other fields as necessary

							
							}
;

VarDecl: TYPE ID SEMICOLON { printf("PARSER: Recognized variable declaration: %s\n", $2);

								printf("PARSER: Printing symbol table\n");
								printSymbolTable(symTab);

								printf("PARSER: Checking if variable has already been declared\n");

								// Check if variable has already been declared
								
								symbol = lookupSymbol(symTab, $2);
							
								if (symbol != NULL) {	// Check if variable has already been declared
									printf("PARSER: Variable %s at line %d has already been declared - COMPILATION HALTED\n", $2, yylineno);
									exit(0);
								} else {	
										// Variable has not been declared yet	
										// Create AST node for VarDecl

										$$ = malloc(sizeof(ASTNode));
										$$->type = NodeType_VarDecl;
										$$->varDecl.varType = strdup($1);
										$$->varDecl.varName = strdup($2);
										// Set other fields as necessary

										// Add variable to symbol table
										addSymbol(symTab, $2, $1);
										printSymbolTable(symTab);
									}
								
							 }
		| TYPE ID {
                  printf ("Missing semicolon after declaring variable: %s\n", $2);
             }

StmtList:  {/*empty, i.e. it is possible not to have any statement*/}
	| Stmt StmtList { printf("PARSER: Recognized statement list\n");
						$$ = malloc(sizeof(ASTNode));
						$$->type = NodeType_StmtList;
						$$->stmtList.stmt = $1;
						$$->stmtList.stmtList = $2;
						// Set other fields as necessary
					}
;

Stmt: ID EQ Expr SEMICOLON { /* code TBD */
								printf("PARSER: Recognized assignment statement\n");
								$$ = malloc(sizeof(ASTNode));
								$$->type = NodeType_AssignStmt;
								$$->assignStmt.varName = strdup($1);
								$$->assignStmt.operator = $2;
								$$->assignStmt.expr = $3;
								// Set other fields as necessary
 }
	| WRITE ID SEMICOLON { printf("PARSER: Recognized write statement\n"); }
;

Expr: Expr BinOp Expr { printf("PARSER: Recognized expression\n");
						$$ = malloc(sizeof(ASTNode));
						$$->type = NodeType_Expr;
						$$->expr.left = $1;
						$$->expr.right = $3;
						$$->expr.operator = $2->binOp.operator;
						
						// Set other fields as necessary
					  }
 					
	| ID { printf("ASSIGNMENT statement \n"); 
			$$ = malloc(sizeof(ASTNode));
			$$->type = NodeType_SimpleID;
			$$->simpleID.name = $1;
			// Set other fields as necessary	
		}
	| NUMBER { 
				printf("PARSER: Recognized number\n");
				$$ = malloc(sizeof(ASTNode));
				$$->type = NodeType_SimpleExpr;
				$$->simpleExpr.number = $1;
				// Set other fields as necessary
			 }
;

BinOp: PLUS {
				printf("PARSER: Recognized binary operator\n");
				$$ = malloc(sizeof(ASTNode));
				$$->type = NodeType_BinOp;
				$$->binOp.operator = *$1;
				// Set other fields as necessary
            }
;

%%

int main() {
    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");

	// Initialize symbol table
	symTab = createSymbolTable(TABLE_SIZE);
    if (symTab == NULL) {
        // Handle error
        return EXIT_FAILURE;
    }
	symbol = malloc(sizeof(Symbol));
	initializeTempVars();

    // Start parsing
    if (yyparse() == 0) {
        // Successfully parsed
		printf("Parsing successful!\n");
        traverseAST(root, 0);
		// Print symbol table for debugging
		printSymbolTable(symTab);
		// Semantic analysis
		printf("\n=== SEMANTIC ANALYSIS ===\n\n");
		semanticAnalysis(root, symTab);
		printf("\n=== TAC GENERATION ===\n");
		printTACToFile("TAC.ir", tacHead);

		// Code optimization
		printf("\n=== CODE OPTIMIZATION ===\n");
		// Traverse the linked list of TAC entries and optimize
		// But - you MIGHT need to traverse the AST again to optimize

		optimizeTAC(&tacHead);
		printOptimizedTAC("TACOptimized.ir", tacHead);

		// Code generation
		printf("\n=== CODE GENERATION ===\n");
		initCodeGenerator("output.s");
		generateMIPS(tacHead);
		finalizeCodeGenerator("output.s");

        freeAST(root);
		freeSymbolTable(symTab);
    } else {
        fprintf(stderr, "Parsing failed\n");
    }

    fclose(yyin);
    return 0;
}

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}