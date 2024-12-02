#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodes.h"
#include "symbol_table.h"

extern SymbolTable *symbol_table;
extern Node *program_root;

Node* create_node(const char *type, Value *value, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro: Falha na alocação de memória para o nó.\n");
        exit(1);
    }

    node->type = strdup(type);
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

void free_node(Node *node) {
    if (!node) return;

    if (node->value) {
        if (node->value->type == TYPE_STR && node->value->data.str_value) {
            free(node->value->data.str_value);
        }
        else if (node->value->type == TYPE_INT) {
            // Nada a liberar
        }
        free(node->value);
    }

    free(node->type);
    free_node(node->left);
    free_node(node->right);
    free(node);
}

Value* evaluate_node(Node *node) {
    if (!node) {
        fprintf(stderr, "Erro: Nó NULL encontrado.\n");
        exit(1);
    }

    if (strcmp(node->type, "IntVal") == 0 || strcmp(node->type, "StrVal") == 0) {
        return node->value;
    } 
    else if (strcmp(node->type, "BinOp") == 0) {
        Value *left_val = evaluate_node(node->left);
        Value *right_val = evaluate_node(node->right);

        // Verificação de tipos para operadores específicos
        if (strcmp(node->value->data.str_value, "+") == 0 || strcmp(node->value->data.str_value, "-") == 0) {
            if (left_val->type != TYPE_INT || right_val->type != TYPE_INT) {
                fprintf(stderr, "Erro: Operadores aritméticos esperam operandos do tipo 'int'.\n");
                exit(1);
            }
            int left = left_val->data.int_value;
            int right = right_val->data.int_value;

            Value *result = (Value *)malloc(sizeof(Value));
            result->type = TYPE_INT;

            if (strcmp(node->value->data.str_value, "+") == 0) {
                result->data.int_value = left + right;
            } else if (strcmp(node->value->data.str_value, "-") == 0) {
                result->data.int_value = left - right;
            }
            return result;
        }
        else if (strcmp(node->value->data.str_value, "<") == 0 || strcmp(node->value->data.str_value, ">") == 0 ||
                strcmp(node->value->data.str_value, "==") == 0 || strcmp(node->value->data.str_value, "!=") == 0) {
            if (left_val->type != right_val->type) {
                fprintf(stderr, "Erro: Comparação entre tipos incompatíveis.\n");
                exit(1);
            }

            int comparison_result = 0;

            if (left_val->type == TYPE_INT) {
                int left = left_val->data.int_value;
                int right = right_val->data.int_value;

                if (strcmp(node->value->data.str_value, "<") == 0) {
                    comparison_result = left < right;
                } else if (strcmp(node->value->data.str_value, ">") == 0) {
                    comparison_result = left > right;
                } else if (strcmp(node->value->data.str_value, "==") == 0) {
                    comparison_result = left == right;
                } else if (strcmp(node->value->data.str_value, "!=") == 0) {
                    comparison_result = left != right;
                }
            } else if (left_val->type == TYPE_STR) {
                int cmp = strcmp(left_val->data.str_value, right_val->data.str_value);

                if (strcmp(node->value->data.str_value, "<") == 0) {
                    fprintf(stderr, "Erro: Operador '<' não suportado para strings.\n");
                    exit(1);
                } else if (strcmp(node->value->data.str_value, ">") == 0) {
                    fprintf(stderr, "Erro: Operador '>' não suportado para strings.\n");
                    exit(1);
                } else if (strcmp(node->value->data.str_value, "==") == 0) {
                    comparison_result = cmp == 0;
                } else if (strcmp(node->value->data.str_value, "!=") == 0) {
                    comparison_result = cmp != 0;
                }
            }

            Value *result = (Value *)malloc(sizeof(Value));
            result->type = TYPE_INT;
            result->data.int_value = comparison_result;
            return result;
        }
        else {
            fprintf(stderr, "Erro: Operador desconhecido '%s'.\n", node->value->data.str_value);
            exit(1);
        }
    }
    else if (strcmp(node->type, "Statements") == 0) {
        if (node->left) {
            evaluate_node(node->left);
        }
        if (node->right) {
            evaluate_node(node->right);
        }
        return NULL;
    }
    else if (strcmp(node->type, "Print") == 0) {
        Value *val = evaluate_node(node->left);
        if (!val) {
            fprintf(stderr, "Erro: Valor nulo encontrado para o nó Print.\n");
            exit(1);
        }
        if (val->type == TYPE_INT) {
            printf("%d\n", val->data.int_value);
        } else if (val->type == TYPE_STR) {
            printf("%s\n", val->data.str_value);
        } else {
            fprintf(stderr, "Erro: Tipo de valor desconhecido para o nó Print.\n");
            exit(1);
        }
        return NULL;
    }
    else if (strcmp(node->type, "If") == 0) {
        Value *cond_val = evaluate_node(node->left);
        if (cond_val->type != TYPE_INT) {
            fprintf(stderr, "Erro: A condição do 'If' deve ser um valor inteiro.\n");
            exit(1);
        }
        if (cond_val->data.int_value) {
            evaluate_node(node->right);
            // Retornar um valor indicando que o bloco foi executado
            Value *result = (Value *)malloc(sizeof(Value));
            result->type = TYPE_INT;
            result->data.int_value = 1;
            return result;
        }
        // Se a condição for falsa, retornar NULL
        return NULL;
    }
    else if (strcmp(node->type, "ElseIf") == 0) {
        Value *cond_val = evaluate_node(node->left);
        if (cond_val->type != TYPE_INT) {
            fprintf(stderr, "Erro: A condição do 'ElseIf' deve ser um valor inteiro.\n");
            exit(1);
        }
        if (cond_val->data.int_value) {
            evaluate_node(node->right);
            // Retornar um valor indicando que o bloco foi executado
            Value *result = (Value *)malloc(sizeof(Value));
            result->type = TYPE_INT;
            result->data.int_value = 1;
            return result;
        }
        // Se a condição for falsa, retornar NULL
        return NULL;
    }
    else if (strcmp(node->type, "IfElseIf") == 0) {
        // Avaliar o nó esquerdo (If ou ElseIf)
        Value *result = evaluate_node(node->left);
        if (result == NULL && node->right) {
            // Se o bloco anterior não foi executado, avaliar o próximo
            return evaluate_node(node->right);
        }
        // Caso contrário, retornar o resultado atual (bloco foi executado)
        return result;
    }
    else if (strcmp(node->type, "Else") == 0) {
        // Executar o bloco else
        evaluate_node(node->left);
        // Retornar um valor indicando que o bloco foi executado
        Value *result = (Value *)malloc(sizeof(Value));
        result->type = TYPE_INT;
        result->data.int_value = 1;
        return result;
    }
    else if (strcmp(node->type, "While") == 0) {
        Value *cond_val = evaluate_node(node->left);
        if (cond_val->type != TYPE_INT) {
            fprintf(stderr, "Erro: A condição do 'While' deve ser um valor inteiro.\n");
            exit(1);
        }
        while (cond_val->data.int_value) {
            evaluate_node(node->right);
            cond_val = evaluate_node(node->left);
            if (cond_val->type != TYPE_INT) {
                fprintf(stderr, "Erro: A condição do 'While' deve ser um valor inteiro.\n");
                exit(1);
            }
        }
        return NULL;
    }
    else if (strcmp(node->type, "VarDecl") == 0) {
        ValueType var_type = node->value->type;
        char *var_name = node->value->data.str_value;
        assign_variable(symbol_table, var_name, var_type, NULL);
        return NULL;
    }
    else if (strcmp(node->type, "VarDeclAssign") == 0) {
        // Declaração de variável com atribuição
        char *var_name = node->left->value->data.str_value;
        ValueType var_type = node->left->value->type;

        // Avaliar expressão
        Value *evaluated_value = evaluate_node(node->right);

        // Verificar compatibilidade de tipos
        if (var_type != evaluated_value->type) {
            fprintf(stderr, "Erro: Tipo incompatível na atribuição à variável '%s' durante a declaração.\n", var_name);
            exit(1);
        }

        // Adicionar variável à tabela de símbolos com o valor inicial
        assign_variable(symbol_table, var_name, var_type, evaluated_value);

        return NULL;
    }
    else if (strcmp(node->type, "VarAssign") == 0) {
        // Atribuição a variável existente
        // Avaliar expressão
        Value *evaluated_value = evaluate_node(node->right);
        // Obter variável da tabela de símbolos
        char *var_name = node->left->value->data.str_value;
        Variable *var = get_variable(symbol_table, var_name);
        if (!var) {
            fprintf(stderr, "Erro: Variável '%s' não declarada.\n", var_name);
            exit(1);
        }
        // Verificar compatibilidade de tipos
        if (var->type != evaluated_value->type) {
            fprintf(stderr, "Erro: Tipo incompatível na atribuição à variável '%s'.\n", var_name);
            exit(1);
        }
        // Atribuir valor
        if (var->type == TYPE_INT) {
            var->int_value = evaluated_value->data.int_value;
        } else if (var->type == TYPE_STR) {
            free(var->str_value);
            var->str_value = strdup(evaluated_value->data.str_value);
        }
        return NULL;
    }
    else if (strcmp(node->type, "Var") == 0) {
        // Obter variável da tabela de símbolos
        char *var_name = node->value->data.str_value;
        Variable *var = get_variable(symbol_table, var_name);
        if (!var) {
            fprintf(stderr, "Erro: Variável '%s' não declarada.\n", var_name);
            exit(1);
        }
        // Retornar valor da variável
        Value *result = (Value *)malloc(sizeof(Value));
        result->type = var->type;
        if (var->type == TYPE_INT) {
            result->data.int_value = var->int_value;
        } else if (var->type == TYPE_STR) {
            result->data.str_value = strdup(var->str_value);
        }
        return result;
    }
    else {
        fprintf(stderr, "Erro: Tipo de nó desconhecido '%s'.\n", node->type);
        exit(1);
    }
    return NULL;
}





