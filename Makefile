all: lex.yy.o scanner.o hash.o util.o
	@echo "Montando o scanner"
	@gcc util.o main.o lex.yy.o hash.o -L . -o etapa1 

scanner.o: main.c
	@echo "Compilar o main.c"
	@gcc -c main.c -o main.o

lex.yy.o: scanner.l
	@echo "Compilar o scanner.l"
	@lex scanner.l
	@gcc -c lex.yy.c -o lex.yy.o

hash.o: hash.c hash.h
	@echo "Compilando a tabela Hash"
	@gcc -c hash.c -o hash.o

util.o: util.c util.h
	@echo "Compilando a tabela Hash"
	@gcc -c util.c -o util.o

clean:
	@rm -f lex.yy.* *.o etapa1 
