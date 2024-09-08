%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()

#define TYPE 1
#define COMMAND 2
#define IDENTIFIER 3
#define INTEGER 4
#define FLOAT 5
#define OPENING_PARENTHESIS 6
#define CLOSING_PARENTHESIS 7
#define OPENING_SQUARE_BRACKET 8
#define CLOSING_SQUARE_BRACKET 9
#define OPENING_CURLY_BRACKET 10
#define CLOSING_CURLY_BRACKET 11
#define SEMICOLON 12
#define EQUALITY_OPERATOR 13
#define EQUALS 14
#define COMMA 15
#define PLUS 16
#define MINUS 17
#define MULTIPLY 18
#define DIVIDE 19
#define GEQ 20
#define LEQ 21
#define LESS_THAN 22
#define GREATER_THAN 23
#define NOT 24
#define AND 25
#define PERIOD 26
#define LINE 27
#define CONDITION 28
#define CHAR 29
#define STRING 30

/* Define YYSTYPE structure */
typedef union {
    char *string;
} YYSTYPE;

/* Declare yylval */
YYSTYPE yylval;

#define MAX_ERRORS 100  // Maximum number of errors to track

int words = 0;
int chars = 0;
int lines = 1;
int errors = 0;
int error_positions[MAX_ERRORS][2];  // Store the line and character positions of errors

/* Function to add a new error */
void add_error(int line, int chars) {
    error_positions[errors][0] = line;
    error_positions[errors][1] = chars;
    errors++;
    }

%}

letter      [a-zA-Z]
digit       [0-9]
ID          {letter}({letter}|{digit})*
delim       [ \t\n]
NUMBER      {digit}+(\.{digit}+)?(E[+\-]?(digit)+)?
ws          {delim}+

%%

"int"               { 
                        words++; chars += strlen(yytext);
                        printf("TYPE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return TYPE; 
                    }

"str"               { 
                        words++; chars += strlen(yytext);
                        printf("TYPE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return TYPE; 
                    }

"float"             { 
                        words++; chars += strlen(yytext);
                        printf("TYPE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return TYPE; 
                    }

"bool"              { 
                        words++; chars += strlen(yytext);
                        printf("TYPE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return TYPE; 
                    }

"char"              { 
                        words++; chars += strlen(yytext);
                        printf("TYPE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return TYPE; 
                    }

"return"            { 
                        words++; chars += strlen(yytext);
                        printf("COMMAND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return COMMAND; 
                    }

"write"             { 
                        words++; chars += strlen(yytext);
                        printf("COMMAND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return COMMAND; 
                    }

"writeln"           { 
                        words++; chars += strlen(yytext);
                        printf("COMMAND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return COMMAND; 
                    }

"read"              { 
                        words++; chars += strlen(yytext);
                        printf("COMMAND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return COMMAND; 
                    }

"break"             { 
                        words++; chars += strlen(yytext);
                        printf("COMMAND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return COMMAND; 
                    }

"ln"                { 
                        words++; chars += strlen(yytext);
                        printf("LINE: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return LINE; 
                    }

"if"                { 
                        words++; chars += strlen(yytext);
                        printf("CONDITION: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CONDITION; 
                    }

"while"             { 
                        words++; chars += strlen(yytext);
                        printf("CONDITION: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CONDITION; 
                    }

"else"              { 
                        words++; chars += strlen(yytext);
                        printf("CONDITION: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CONDITION; 
                    }

[a-zA-Z_][a-zA-Z0-9_]* { 
                        words++; chars += strlen(yytext);
                        printf("IDENTIFIER: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return IDENTIFIER; 
                    }

[0-9]+\.[0-9]+      { 
                        chars += strlen(yytext);
                        printf("FLOAT: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return FLOAT; 
                    }

[0-9]+              { 
                        chars += strlen(yytext);
                        printf("INTEGER: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return INTEGER; 
                    }

"("                 { 
                        chars ++;
                        printf("OPENING_PARENTHESIS: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return OPENING_PARENTHESIS; 
                    }

")"                 { 
                        chars ++;
                        printf("CLOSING_PARENTHESIS: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CLOSING_PARENTHESIS; 
                    }

"["                 { 
                        chars ++;
                        printf("OPENING_SQUARE_BRACKET: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return OPENING_SQUARE_BRACKET; 
                    }

"]"                 { 
                        chars ++;
                        printf("CLOSING_SQUARE_BRACKET: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CLOSING_SQUARE_BRACKET; 
                    }

"{"                 { 
                        chars ++;
                        printf("OPENING_CURLY_BRACKET: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return OPENING_CURLY_BRACKET; 
                    }

"}"                 { 
                        chars ++;
                        printf("CLOSING_CURLY_BRACKET: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return CLOSING_CURLY_BRACKET; 
                    }

";"                 { 
                        chars ++;
                        printf("SEMICOLON: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return SEMICOLON; 
                    }

"=="                { 
                        chars +=2;
                        printf("EQUALITY_OPERATOR: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return EQUALITY_OPERATOR; 
                    }

"="                 { 
                        chars ++;
                        printf("EQUALS: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
		                return EQUALS;
                    }

","                 { 
                        chars ++;
                        printf("COMMA: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
                        return COMMA;
                    }

"+"                 {   
                        chars ++;
                        printf("PLUS: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
                        return PLUS;
                    }

"-"                 {   
                        chars ++;
                        printf("MINUS: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
                        return MINUS;
                    }

"*"                 {   
                        chars ++;
                        printf("MULTIPLY: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
                        return MULTIPLY;
                    }

"/"                 {   
                        chars ++;
                        printf("DIVIDE: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
                        return DIVIDE;
                    }

">="                { 
                        chars +=2;
                        printf("GEQ: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return GEQ; 
                    }

"<="                { 
                        chars +=2;
                        printf("LEQ: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return LEQ; 
                    }

"<"                 { 
                        chars ++;
                        printf("LESS_THAN: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return LESS_THAN; 
                    }

">"                 { 
                        chars ++;
                        printf("GREATER_THAN: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return GREATER_THAN; 
                    }

"!"                 { 
                        chars ++;
                        printf("NOT: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return NOT; 
                    }

"&&"                { 
                        chars += 2;
                        printf("AND: %s\n", yytext);
                        yylval.string = strdup(yytext); 
			            return AND; 
                    }

\.                  { 
                        chars ++;
                        printf("PERIOD: %s\n", yytext); 
                        yylval.string = strdup(yytext); 
			            return PERIOD; 
                    }

[\n]	            {lines++; chars=0;}
[ \t]	            {chars++;}

.                   { 
                        chars += strlen(yytext);
                        printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
                        add_error(lines, chars);  // Add error to the array
		 
                    } 

%%

int main(int argc, char **argv) {
    ++argv, --argc;  /* skip over program name */
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;

    /* Call yylex() repeatedly to process all tokens */
    while(yylex() != 0) {
        // Continue scanning tokens until the end of the input
    }

    if (errors ==0)
        printf("Source code is correct. No lexical errors found.\n");
    else
        printf("Lexical errors encountered: %d at the following positions:\n", errors);
        printf("[");
        for (int i = 0; i < errors; i++) {
            printf("[%d, %d]", error_positions[i][0], error_positions[i][1]);
            if (i < errors - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    return 0;
}


