#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <llvm-c/Core.h>
#include "nodes.h"

// Estrutura para representar uma entrada na tabela de símbolos
typedef struct {
    char *name;
    LLVMValueRef value;
    ValueType type; // Adiciona o tipo aqui
} Symbol;

// Estrutura para representar a tabela de símbolos
typedef struct {
    Symbol *symbols;      // Array de símbolos
    int count;            // Número atual de símbolos
    int capacity;         // Capacidade total da tabela
} SymbolTable;

// Declaração da variável global da tabela de símbolos
extern SymbolTable *symbol_table;

// Funções para manipular a tabela de símbolos
SymbolTable *init_symbol_table();
void free_symbol_table();
void insert_symbol(const char *name, LLVMValueRef value, ValueType type);
Symbol *lookup_symbol(char *name);

#endif // SYMBOL_TABLE_H




