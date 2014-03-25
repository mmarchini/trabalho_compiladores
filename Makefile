all: util.o  hash.o y.tab.o lex.yy.o scanner.o
	@echo "Montando o scanner"
	@gcc util.o main.o lex.yy.o y.tab.o hash.o -L . -o etapa2 

util.o: util.c util.h
	@echo "Compilando a tabela Hash"
	@gcc -c util.c -o util.o

scanner.o: main.c
	@echo "Compilar o main.c"
	@gcc -c main.c -o main.o

y.tab.o: syntax.y
	@echo "Compilar o syntax.y"
	@yacc syntax.y -d
	@gcc -c y.tab.c -o y.tab.o

lex.yy.o: scanner.l
	@echo "Compilar o scanner.l"
	@lex scanner.l
	@gcc -c lex.yy.c -o lex.yy.o

hash.o: hash.c hash.h
	@echo "Compilando a tabela Hash"
	@gcc -c hash.c -o hash.o

clean:
	@rm -f lex.yy.* *.o etapa2 y.tab.* 
