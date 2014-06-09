ETAPA=etapa5

CC=gcc
CCFLAGS=--std=c99 -L .

DEPS=util ast hash main semantic
DEP_OBJS:=${foreach file, $(DEPS),$(file).o}

LEX=lex
LEX_FILE=scanner.l 
LEX_OBJ=lex.yy.o

YACC=bison
YACCFLAGS=-d --file-prefix y
YACC_FILE=syntax.y 
YACC_OBJ=y.tab.o

all: $(YACC_OBJ) $(LEX_OBJ) $(DEP_OBJS)  
	@echo "Compilando o Compilador"
	@$(CC) $(YACC_OBJ) $(LEX_OBJ) $(DEP_OBJS) -o $(ETAPA) $(CCFLAGS)

%.o: %.c $(DEPS)
	@$(CC) -c -o $@ $(CCFLAGS)


$(YACC_OBJ): $(YACC_FILE)
	@echo "Compilando o $(YACC_FILE)"
	@$(YACC) syntax.y $(YACCFLAGS)
	@$(CC) -c y.tab.c -o y.tab.o $(CCFLAGS)

$(LEX_OBJ): $(LEX_FILE) 
	@echo "Compilando o $(LEX_OBJ)"
	@$(LEX) scanner.l
	@$(CC) -c lex.yy.c -o lex.yy.o $(CCFLAGS)

test:
	@echo "Testing Syntax"
	@./tests/syntax.sh $(ETAPA)
	@echo "Testing Semantics"
	@./tests/semantic.sh $(ETAPA)

clean:
	@rm -f lex.yy.* *.o etapa3 y.tab.* 
