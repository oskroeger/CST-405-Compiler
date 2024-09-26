%{

#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);
%}

%union {
	int number;
	char character;
	char* string;
}

%token <string> TYPE
%token <string> ID
%token <char> SEMICOLON
%token <char> EQ
%token <char> COMMA
%token <char> PLUS
%token <number> NUMBER
%token <string> WRITE


%printer { fprintf(yyoutput, "%s", $$); } ID;


%start Program

%%

Program: VarDeclList StmtList    { printf("The PARSER has started\n"); }
;

VarDeclList:  {/*empty, i.e. it is possible not to declare a variable*/}
	| VarDecl VarDeclList { /* code TBD */ }
;

VarDecl: TYPE ID SEMICOLON { printf("PARSER: Recognized variable declaration: %s\n", $2); }
		| TYPE ID {
                  printf ("Missing semicolon after declaring variable: %s\n", $2);
             }

StmtList:  {/*empty, i.e. it is possible not to have any statement*/}
	| Stmt StmtList { /* code TBD */ }
;

Stmt: ID EQ Expr SEMICOLON { /* code TBD */ }
	| WRITE ID SEMICOLON { printf("PARSER: Recognized write statement\n"); }
;

Expr: Expr BinOp Expr { /* code TBD */ }
	| ID { printf("ASSIGNMENT statement /n"); }
	| NUMBER { /* code TBD */ }
;

BinOp: PLUS { /* code TBD */ }
;

%%

int main(int argc, char**argv)
{
/*
	#ifdef YYDEBUG
		yydebug = 1;
	#endif
*/
	printf("Compiler started. \n\n");
	
	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}
	yyparse();
}

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
