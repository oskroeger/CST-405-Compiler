%option noyywrap

%{
#include <stdio.h>
#include <string.h>
#include "mini_parser.tab.h"

#define YY_DECL int yylex()
extern YYSTYPE yylval;

int words = 0;
int chars = 0;
int lines = 0;

%}

letter      [a-zA-Z]
digit       [0-9]
ID          {letter}({letter}|{digit})*
delim       [ \t\n]
NUMBER      {digit}+(\.{digit}+)?(E[+\-]?(digit)+)?
ws          {delim}+

%%
"/*"    				{
							int c;
							while((c = input()) != 0) {
								if(c == '*') {
									if((c = input()) == '/')
										break;
									else
										unput(c);
								}
							}
						}
						
"int"	{
			words++; chars += strlen(yytext);
			printf("%s : TYPE\n", yytext);
			yylval.string = strdup(yytext); 
			return TYPE;
		}

"write"	{
			words++; chars += strlen(yytext);
			printf("%s : KEYWORD\n", yytext);
			yylval.string = strdup(yytext); 
			return WRITE;
		}

{ID}	{
			  words++; chars += strlen(yytext);
			  printf("%s : IDENTIFIER\n",yytext);
			  yylval.string = strdup(yytext); 
			  return ID;
			}
			
{NUMBER}	{
			  words++; chars += strlen(yytext);
			  printf("%s : NUMBER\n",yytext);
			  yylval.number = atoi(strdup(yytext)); 
			  return NUMBER;
			}
			
";"		{
		  chars++;
		  printf("%s : SEMICOLON\n", yytext);
		  yylval.string = strdup(yytext); 
		  return SEMICOLON;
		}

","		{
		  chars++;
		  printf("%s : COMMA\n", yytext);
		  yylval.string = strdup(yytext); 
		  return COMMA;
		}
		
"="		{
		  chars++;
		  printf("%s : EQ\n", yytext);
		  yylval.string = strdup(yytext); 
		  return EQ;
		}

"+"		{
		  chars++;
		  printf("%s : PLUS\n", yytext);
		  yylval.string = strdup(yytext); 
		  return PLUS;
		}		
		
[\n]	{lines++; chars=0;}
[ \t]	{chars++;}
.		{chars++;
         printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}
%%