all: lex.yy.o analisador.o
	@echo "Montando o analisador"
	@gcc main.o lex.yy.o -o etapa1 

analisador.o: main.c
	@echo "Compilar o main.c"
	@gcc -c main.c -o main.o

lex.yy.o: analisador.l
	@echo "Compilar o analisador.l"
	@lex analisador.l
	@gcc -c lex.yy.c -o lex.yy.o

clean:
	@rm -f lex.yy.* *.o analisador
