%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()
#include "parser.tab.h"

int words = 0;
int chars = 0;
int lines = 1; // Start line counting from 1

%}

letter      [a-zA-Z]
digit       [0-9]
ID          {letter}({letter}|{digit})*
delim       [ \t\n]
NUMBER      {digit}+(\.{digit}+)?(E[+\-]?(digit)+)?
ws          {delim}+

%option yylineno
%%
"/*" {
        int c;
        while ((c = input()) != 0) {
            if (c == '*') {
                if ((c = input()) == '/')
                    break;
                else
                    unput(c);
            }
        }
    }

"int" {
        words++;
        chars += strlen(yytext);
        printf("%s : TYPE\n", yytext);
        yylval.string = strdup(yytext);
        return TYPE;
    }

"write" {
        words++;
        chars += strlen(yytext);
        printf("%s : KEYWORD\n", yytext);
        yylval.string = strdup(yytext);
        return WRITE;
    }

{ID} {
        words++;
        chars += strlen(yytext);
        printf("%s : IDENTIFIER\n", yytext);
        yylval.string = strdup(yytext);
        return ID;
    }

{NUMBER} {
        words++;
        chars += strlen(yytext);
        printf("%s : NUMBER\n", yytext);
        yylval.number = atoi(strdup(yytext));
        return NUMBER;
    }

";" {
        chars++;
        printf("%s : SEMICOLON\n", yytext);
        yylval.string = strdup(yytext);
        return SEMICOLON;
    }

"=" {
        chars++;
        printf("%s : EQ\n", yytext);
        yylval.operator = strdup(yytext);
        return EQ;
    }

"+" {
        chars++;
        printf("%s : PLUS\n", yytext);
        yylval.operator = strdup(yytext);
        return PLUS;
    }

"-" {
        chars++;
        printf("%s : MINUS\n", yytext);
        yylval.operator = strdup(yytext);
        return MINUS;
    }

"*" {
        chars++;
        printf("%s : MULTIPLY\n", yytext);
        yylval.operator = strdup(yytext);
        return MULTIPLY;
    }

"/" {
        chars++;
        printf("%s : DIVIDE\n", yytext);
        yylval.operator = strdup(yytext);
        return DIVIDE;
    }

"(" {
        chars++;
        printf("%s : LPAREN\n", yytext);
        yylval.string = strdup(yytext);
        return LPAREN;
    }

")" {
        chars++;
        printf("%s : RPAREN\n", yytext);
        yylval.string = strdup(yytext);
        return RPAREN;
    }

\n {
        lines++; 
        chars = 0; 
    }

[ \t] { chars++; }

. {
        chars++;
        printf("%s : Unrecognized symbol at line %d char %d\n", yytext, lines, chars);
    }

%%
