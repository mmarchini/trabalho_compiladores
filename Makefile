
EXTRA_PARAMS=--std=c99

all: util.o ast.o hash.o y.tab.o lex.yy.o scanner.o
	@echo "Montando o scanner"
	@gcc ${EXTRA_PARAMS} util.o ast.o main.o lex.yy.o y.tab.o hash.o -L . -o etapa3 

util.o: util.c util.h
	@echo "Compilando a tabela Hash"
	@gcc ${EXTRA_PARAMS} -c util.c -o util.o

scanner.o: main.c
	@echo "Compilar o main.c"
	@gcc ${EXTRA_PARAMS} -c main.c -o main.o

y.tab.o: syntax.y
	@echo "Compilar o syntax.y"
	@bison syntax.y -d --file-prefix y
	@gcc ${EXTRA_PARAMS} -c y.tab.c -o y.tab.o

lex.yy.o: scanner.l
	@echo "Compilar o scanner.l"
	@lex scanner.l
	@gcc ${EXTRA_PARAMS} -c lex.yy.c -o lex.yy.o

hash.o: hash.c hash.h
	@echo "Compilando a tabela Hash"
	@gcc ${EXTRA_PARAMS} -c hash.c -o hash.o

ast.o: ast.c ast.h
	@echo "Compilando a tabela ast"
	@gcc ${EXTRA_PARAMS} -c ast.c -o ast.o

clean:
	@rm -f lex.yy.* *.o etapa3 y.tab.* 
