// nodes.c

#include "nodes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Cria um novo valor
Node* create_node(char *type, Value *value, Node *left, Node *right) {
    Node *node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro: Falha ao alocar memória para Node.\n");
        exit(1);
    }
    node->type = strdup(type);
    node->value = value;
    node->left = left;
    node->right = right;
    node->next = NULL;
    return node;
}

// Cria um novo valor
Value* create_value(ValueType type, char *str) {
    Value *val = malloc(sizeof(Value));
    if (!val) {
        fprintf(stderr, "Erro: Falha ao alocar memória para Value.\n");
        exit(1);
    }
    val->type = type;
    if (type == TYPE_STR && str != NULL) {
        val->data.str_value = strdup(str);
    } else {
        val->data.str_value = NULL;
    }
    return val;
}

// Libera a memória alocada para um nó e seus filhos
void free_node(Node *node) {
    if (!node) return;

    if (node->type) free(node->type);
    if (node->value) {
        if (node->value->type == TYPE_STR && node->value->data.str_value) {
            free(node->value->data.str_value);
        }
        free(node->value);
    }
    free_node(node->left);
    free_node(node->right);
    free(node);
}
