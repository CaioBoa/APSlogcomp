#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "nodes.h" // Para usar ValueType

typedef struct Variable {
    char *name;
    ValueType type;
    int int_value;
    char *str_value;
    struct Variable *next;
} Variable;

typedef struct SymbolTable {
    Variable *variables;
    struct SymbolTable *parent;
} SymbolTable;

SymbolTable* create_symbol_table(SymbolTable *parent);
void destroy_symbol_table(SymbolTable *table);
void assign_variable(SymbolTable *table, const char *name, ValueType type, Value *initial_value);
Variable* get_variable(SymbolTable *table, const char *name);
int exists_in_current_scope(SymbolTable *table, const char *name);
Variable* create_variable(ValueType type, int int_value, const char *str_value);
void free_variable(Variable *var);
void set_variable(SymbolTable *table, const char *name, void *value);

extern SymbolTable *symbol_table;

#endif // SYMBOL_TABLE_H


