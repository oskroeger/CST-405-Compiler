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
NUMBER      {digit}+(\.{digit}+)?([eE][+\-]?{digit}+)?
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

"function" {
        words++;
        chars += strlen(yytext);
        printf("%s : KEYWORD\n", yytext);
        yylval.string = strdup(yytext);
        return FUNCTION;
    }

"return" {
        words++;
        chars += strlen(yytext);
        printf("%s : KEYWORD\n", yytext);
        yylval.string = strdup(yytext);
        return RETURN;
    }

"int" {
        words++;
        chars += strlen(yytext);
        printf("%s : TYPE\n", yytext);
        yylval.string = strdup(yytext);
        return TYPE;
    }

"float" {
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

"if" {
    words++;
    chars += strlen(yytext);
    printf("%s : KEYWORD\n", yytext);
    yylval.string = strdup(yytext);
    return IF;
}

"else" {
    words++;
    chars += strlen(yytext);
    printf("%s : KEYWORD\n", yytext);
    yylval.string = strdup(yytext);
    return ELSE;
}

{ID} {
        words++;
        chars += strlen(yytext);
        printf("%s : IDENTIFIER\n", yytext);
        yylval.string = strdup(yytext);
        return ID;
    }

{digit}+\.{digit}+([eE][+\-]?{digit}+)? {
        words++;
        chars += strlen(yytext);
        printf("%s : FLOAT\n", yytext);
        yylval.floatNum = atof(yytext);
        return FLOAT;
    }

{digit}+([eE][+\-]?{digit}+)? {
        words++;
        chars += strlen(yytext);
        printf("%s : INTEGER\n", yytext);
        yylval.integer = atoi(yytext);
        return INTEGER;
    }

";" {
        chars++;
        printf("%s : SEMICOLON\n", yytext);
        yylval.character = yytext[0];
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
        yylval.character = yytext[0];
        return LPAREN;
    }

")" {
        chars++;
        printf("%s : RPAREN\n", yytext);
        yylval.character = yytext[0];
        return RPAREN;
    }

"[" {
        chars++;
        printf("%s : LBRACKET\n", yytext);
        yylval.character = yytext[0];
        return LBRACKET;
    }

"]" {
        chars++;
        printf("%s : RBRACKET\n", yytext);
        yylval.character = yytext[0];
        return RBRACKET;
    }

"{" {
        chars++;
        printf("%s : LBRACE\n", yytext);
        yylval.character = yytext[0];
        return LBRACE;
    }

"}" {
        chars++;
        printf("%s : RBRACE\n", yytext);
        yylval.character = yytext[0];
        return RBRACE;
    }

"==" {
    chars += strlen(yytext);
    printf("%s : EQEQ\n", yytext);
    yylval.operator = strdup(yytext);
    return EQEQ;
}

"!=" {
    chars += strlen(yytext);
    printf("%s : NEQ\n", yytext);
    yylval.operator = strdup(yytext);
    return NEQ;
}

"<=" {
    chars += strlen(yytext);
    printf("%s : LE\n", yytext);
    yylval.operator = strdup(yytext);
    return LE;
}

">=" {
    chars += strlen(yytext);
    printf("%s : GE\n", yytext);
    yylval.operator = strdup(yytext);
    return GE;
}

"<" {
    chars += strlen(yytext);
    printf("%s : LT\n", yytext);
    yylval.operator = strdup(yytext);
    return LT;
}

">" {
    chars += strlen(yytext);
    printf("%s : GT\n", yytext);
    yylval.operator = strdup(yytext);
    return GT;
}

\n {
        lines++; 
        chars = 0; 
    }

[ \t]+ { chars += yyleng; } // Consume whitespace and update character count

. {
        chars++;
        printf("%s : Unrecognized symbol at line %d char %d\n", yytext, yylineno, chars);
        return UNRECOGNIZED;
    }

%%
