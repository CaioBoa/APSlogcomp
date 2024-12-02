# Makefile para o compilador com LLVM

# Compilador e flags
CC = gcc
CFLAGS = -g -Wall

# Flags do LLVM
LLVM_CONFIG = llvm-config
LLVM_CFLAGS = $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --libs core) $(shell $(LLVM_CONFIG) --system-libs)

# Fontes e objetos
SOURCES = lex.yy.c parser.tab.c nodes.c symbol_table.c codegen.c main.c
OBJECTS = $(SOURCES:.c=.o)

# Arquivo executável
EXECUTABLE = my_language

# Regras principais
all: check-tools $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl $(LLVM_LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) -c $< -o $@

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) lex.yy.c parser.tab.c parser.tab.h

# Verifica ferramentas essenciais
check-tools:
	@command -v $(LLVM_CONFIG) >/dev/null 2>&1 || { echo >&2 "Erro: llvm-config não encontrado."; exit 1; }
	@command -v bison >/dev/null 2>&1 || { echo >&2 "Erro: bison não encontrado."; exit 1; }
	@command -v flex >/dev/null 2>&1 || { echo >&2 "Erro: flex não encontrado."; exit 1; }



