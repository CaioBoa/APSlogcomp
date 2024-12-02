// nodes.h

#ifndef NODES_H
#define NODES_H

#include <llvm-c/Core.h>

// Tipos de valores
typedef enum {
    TYPE_INT,
    TYPE_STR
} ValueType;

// Estrutura para representar um valor
typedef struct {
    ValueType type;
    union {
        int int_value;
        char *str_value;
    } data;
} Value;

// Estrutura para representar um nó da AST
typedef struct Node {
    char *type;            // Tipo do nó (e.g., "IntVal", "BinOp", etc.)
    Value *value;          // Valor associado ao nó (se aplicável)
    struct Node *left;     // Filho esquerdo
    struct Node *right;    // Filho direito
} Node;

// Funções para criar nós e valores
Value *create_value(ValueType type, char *str_value);
Node *create_node(char *type, Value *value, Node *left, Node *right);

// Função para liberar memória dos nós
void free_node(Node *node);

#endif // NODES_H
