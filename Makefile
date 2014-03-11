all: lex.yy.o analisador.o hash.o
	@echo "Montando o analisador"
	@gcc main.o lex.yy.o hash.o -L . -o etapa1 

analisador.o: main.c
	@echo "Compilar o main.c"
	@gcc -std=c99 -c main.c -o main.o

lex.yy.o: analisador.l
	@echo "Compilar o analisador.l"
	@lex analisador.l
	@gcc -std=c99 -c lex.yy.c -o lex.yy.o

hash.o: hash.c hash.h
	@echo "Compilando a tabela Hash"
	@gcc -std=c99 -c hash.c -o hash.o

clean:
	@rm -f lex.yy.* *.o etapa1 
