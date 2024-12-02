# Nome do executável
EXEC = my_language

# Compilador e flags
CC = gcc
CFLAGS = -Wall -g -Wno-unused-function

# Bison e Flex
BISON = bison
FLEX = flex

# Arquivos Flex e Bison
LEX_FILE = lexer.l
YACC_FILE = parser.y

# Arquivos gerados pelo Flex e Bison
LEX_C = lex.yy.c
YACC_C = parser.tab.c
YACC_H = parser.tab.h

# Arquivos de código fonte
SRCS = main.c symbol_table.c nodes.c

# Objetos correspondentes
OBJS = lex.yy.o parser.tab.o $(SRCS:.c=.o)

CFLAGS += -DYYDEBUG=1

# Regra padrão
all: $(EXEC)

# Regra para gerar o executável
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

# Regra para compilar arquivos .o a partir de .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para o lex.yy.c
$(LEX_C): $(LEX_FILE) $(YACC_H)
	$(FLEX) $(LEX_FILE)

# Regra para o parser.tab.c e parser.tab.h
$(YACC_C) $(YACC_H): $(YACC_FILE)
	$(BISON) -d $(YACC_FILE)

# Limpar arquivos gerados
clean:
	rm -f $(EXEC) $(OBJS) $(LEX_C) $(YACC_C) $(YACC_H) parser.output

# Alvo para depurar o Bison
bison-debug:
	$(BISON) -d -v --debug $(YACC_FILE)

# Alvo para recompilar com depuração
debug: clean bison-debug $(EXEC)

# Regra para executar o programa com um arquivo de teste
run: $(EXEC)
	./$(EXEC) < teste1.txt

.PHONY: all clean run bison-debug debug


