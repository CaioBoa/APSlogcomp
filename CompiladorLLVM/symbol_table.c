#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Inicializa a tabela de símbolos
SymbolTable *init_symbol_table() {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->count = 0;
    table->capacity = 10;
    table->symbols = (Symbol *)malloc(table->capacity * sizeof(Symbol));
    return table;
}

// Libera a tabela de símbolos
void free_symbol_table() {
    if (!symbol_table) return;
    for (int i = 0; i < symbol_table->count; i++) {
        free(symbol_table->symbols[i].name);
    }
    free(symbol_table->symbols);
    free(symbol_table);
    symbol_table = NULL;
}

// Insere um símbolo na tabela
void insert_symbol(const char *name, LLVMValueRef value, ValueType type) {
    if (!symbol_table) symbol_table = init_symbol_table();
    if (symbol_table->count >= symbol_table->capacity) {
        symbol_table->capacity *= 2;
        symbol_table->symbols = (Symbol *)realloc(symbol_table->symbols, symbol_table->capacity * sizeof(Symbol));
    }
    symbol_table->symbols[symbol_table->count].name = strdup(name);
    symbol_table->symbols[symbol_table->count].value = value;
    symbol_table->symbols[symbol_table->count].type = type;
    symbol_table->count++;
}

// Procura um símbolo na tabela
Symbol *lookup_symbol(char *name) {
    for (int i = 0; i < symbol_table->count; i++) {
        if (strcmp(symbol_table->symbols[i].name, name) == 0) {
            return &symbol_table->symbols[i];
        }
    }
    return NULL;
}





