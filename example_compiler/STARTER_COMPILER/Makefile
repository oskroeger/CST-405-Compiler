all: parser

mini_parser.tab.c mini_parser.tab.h:	mini_parser.y
	bison -t -v -d mini_parser.y

lex.yy.c: lexer.l mini_parser.tab.h
	flex lexer.l

parser: lex.yy.c mini_parser.tab.c mini_parser.tab.h
	gcc -o parser mini_parser.tab.c lex.yy.c
	./parser testProg.cmm

clean:
	rm -f parser lexer mini_parser.tab.c mini_parser.tab.c lex.yy.c mini_parser.tab.h mini_parser.output
	ls -l
