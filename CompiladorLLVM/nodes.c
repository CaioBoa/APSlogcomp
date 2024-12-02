// nodes.c

#include "nodes.h"
#include <stdlib.h>
#include <string.h>

// Cria um novo valor
Value *create_value(ValueType type, char *str_value) {
    Value *val = (Value *)malloc(sizeof(Value));
    val->type = type;
    if (type == TYPE_INT) {
        val->data.int_value = 0;
    } else if (type == TYPE_STR) {
        val->data.str_value = strdup(str_value);
    }
    return val;
}

// Cria um novo nó
Node *create_node(char *type, Value *value, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = strdup(type);
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
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
