lexer: lexer-2.l
	flex lexer-2.l
	gcc -o Lexer lex.yy.c -ll
	./Lexer lexerAnalyzer.gcupl
	
clean:
	rm -f Lexer
	rm -f lex.yy.c
	rm -f a.out
	ls -l

