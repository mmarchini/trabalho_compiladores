all: lex.yy.o analisador.o hash.o util.o
	@echo "Montando o analisador"
	@gcc util.o main.o lex.yy.o hash.o -L . -o etapa1 

analisador.o: main.c
	@echo "Compilar o main.c"
	@gcc -c main.c -o main.o

lex.yy.o: analisador.l
	@echo "Compilar o analisador.l"
	@lex analisador.l
	@gcc -c lex.yy.c -o lex.yy.o

hash.o: hash.c hash.h
	@echo "Compilando a tabela Hash"
	@gcc -c hash.c -o hash.o

util.o: util.c util.h
	@echo "Compilando a tabela Hash"
	@gcc -c util.c -o util.o

clean:
	@rm -f lex.yy.* *.o etapa1 
