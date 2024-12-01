#ifndef NODES_H
#define NODES_H

typedef enum {
    TYPE_INT,
    TYPE_STR,
    TYPE_NONE
} ValueType;

typedef struct {
    ValueType type;
    union {
        int int_value;
        char *str_value;
    } data;
} Value;

typedef struct Node {
    char *type;
    Value *value;
    struct Node *left;
    struct Node *right;
} Node;

Node* create_node(const char *type, Value *value, Node *left, Node *right);
void free_node(Node *node);
Value* evaluate_node(Node *node);

#endif





