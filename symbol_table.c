#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "nodes.h"

const char* value_type_to_string(ValueType type) {
    switch (type) {
        case TYPE_INT:
            return "int";
        case TYPE_STR:
            return "str";
        case TYPE_NONE:
            return "none";
        default:
            return "unknown";
    }
}

SymbolTable* create_symbol_table(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Erro: Falha na alocação da tabela de símbolos.\n");
        exit(1);
    }
    table->variables = NULL;
    table->parent = parent;
    return table;
}

void assign_variable(SymbolTable *table, const char *name, ValueType type, Value *initial_value) {
    if (!table) {
        fprintf(stderr, "Erro: Tabela de símbolos não inicializada.\n");
        exit(1);
    }

    Variable *var = get_variable(table, name);
    if (var) {
        // Variável já existe, verificar tipo
        if (var->type != type) {
            fprintf(stderr, "Erro: Redefinição da variável '%s' com tipo diferente.\n", name);
            exit(1);
        }
    } else {
        // Criar nova variável
        var = (Variable *)malloc(sizeof(Variable));
        var->name = strdup(name);
        var->type = type;
        var->int_value = 0;
        var->str_value = NULL;
        var->next = table->variables;
        table->variables = var;
    }

    // Atribuir valor inicial, se fornecido
    if (initial_value) {
        if (var->type != initial_value->type) {
            fprintf(stderr, "Erro: Tipo incompatível na inicialização da variável '%s'.\n", name);
            exit(1);
        }
        if (var->type == TYPE_INT) {
            var->int_value = initial_value->data.int_value;
        } else if (var->type == TYPE_STR) {
            var->str_value = strdup(initial_value->data.str_value);
        }
    }
}


Variable* get_variable(SymbolTable *table, const char *name) {
    SymbolTable *current = table;
    while (current != NULL) {
        Variable *var = current->variables;
        while (var != NULL) {
            if (strcmp(var->name, name) == 0) {
                return var;
            }
            var = var->next;
        }
        current = current->parent; // Verifica o escopo pai
    }
    return NULL; // Variável não encontrada
}

void destroy_symbol_table(SymbolTable *table) {
    if (!table) return;

    Variable *current = table->variables;
    while (current) {
        Variable *next = current->next;
        free_variable(current);
        current = next;
    }

    free(table);
}

void free_variable(Variable *var) {
    if (!var) return;
    free(var->name);
    // Não precisamos liberar var->type, pois é um enum
    if (var->str_value) free(var->str_value);
    free(var);
}

void set_variable(SymbolTable *table, const char *name, void *value) {
    if (!table) {
        fprintf(stderr, "Erro: Tabela de símbolos não inicializada.\n");
        exit(1);
    }

    Variable *var = get_variable(table, name);
    if (!var) {
        fprintf(stderr, "Erro: Variável '%s' não encontrada na tabela de símbolos.\n", name);
        SymbolTable *curr = table;
        printf("Tabela de símbolos atual:\n");
        while (curr) {
            Variable *v = curr->variables;
            while (v) {
                printf(" - Variável: %s (tipo: %s)\n", v->name, value_type_to_string(v->type));
                v = v->next;
            }
            curr = curr->parent;
        }
        exit(1);
    }

    if (var->type == TYPE_INT) {
        var->int_value = *((int *)value);
    } else if (var->type == TYPE_STR) {
        free(var->str_value);
        var->str_value = strdup((char *)value);
    } else {
        fprintf(stderr, "Erro: Tipo desconhecido para variável '%s'.\n", name);
        exit(1);
    }
}



