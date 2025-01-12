all: parser

parser.tab.c parser.tab.h: parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser: lex.yy.c parser.tab.c parser.tab.h AST.c symbolTable.c semantic.c optimizer.c codeGenerator.c
	gcc -o parser parser.tab.c lex.yy.c AST.c symbolTable.c semantic.c optimizer.c codeGenerator.c -Wall
	./parser testProg.cmm

clean:
	rm -f parser parser.tab.c lex.yy.c parser.tab.h parser.output lex.yy.o parser.tab.o AST.o symbolTable.o semantic.o optimizer.o codeGenerator.o
	ls -l
