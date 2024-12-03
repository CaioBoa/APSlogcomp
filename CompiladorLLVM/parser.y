%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%code requires {
    #include "nodes.h"
}

%code {
    #include "symbol_table.h"
    #include "codegen.h"

    // Variável global para a raiz da AST
    extern Node *program_root;

    // Protótipo de yylex
    int yylex(void);

    // Função para tratamento de erros
    void yyerror(const char *s) {
        fprintf(stderr, "Erro de sintaxe: %s\n", s);
        exit(1);
    }
}

%union {
    int num;
    char *str;
    Node *node;
}

%token <num> NUM
%token <str> VAR
%token <str> TYPE
%token <str> PREVAR
%token <str> ASSIGN
%token <str> OP_ADD
%token <str> OP_SUB
%token <str> OP_MUL
%token <str> OP_DIV
%token <str> OP_LT
%token <str> OP_GT
%token <str> OP_EQ
%token <str> OP_NE
%token <str> IF
%token <str> ELSE
%token <str> ELSEIF
%token <str> WHILE
%token <str> PRINT
%token <str> LPAREN
%token <str> RPAREN
%token <str> LBRACE
%token <str> RBRACE
%token <str> ENDLINE
%token <str> STR

%type <node> program block statements statement elseif_list expression equality_expression relational_expression additive_expression term factor

%%

program:
    block
    {
        program_root = $1;
    }
    ;

block:
    LBRACE statements RBRACE
    {
        $$ = create_node("Block", NULL, $2, NULL);
    }
    ;

statements:
    statements statement
    {
        $$ = create_node("Statements", NULL, $1, $2);
    }
    | /* vazio */
    {
        $$ = NULL;
    }
    ;

statement:
    TYPE VAR ENDLINE
    {
        // Cria o valor associado ao nome da variável
        Value *var_value = (Value *)malloc(sizeof(Value));
        if (!var_value) {
            fprintf(stderr, "Erro: Falha ao alocar memória para Value.\n");
            exit(1);
        }

        // Define o tipo da variável com base no tipo declarado
        if (strcmp($1, "str") == 0) {
            var_value->type = TYPE_STR;
        } else if (strcmp($1, "int") == 0) {
            var_value->type = TYPE_INT;
        } else {
            fprintf(stderr, "Erro: Tipo desconhecido '%s'.\n", $1);
            exit(1);
        }

        // Copia o nome da variável
        var_value->data.str_value = strdup($2);
        if (!var_value->data.str_value) {
            fprintf(stderr, "Erro: Falha ao copiar o nome da variável.\n");
            exit(1);
        }

        // Cria o nó de declaração de variável
        $$ = create_node("VarDecl", var_value, NULL, NULL);
        if (!$$) {
            fprintf(stderr, "Erro: Falha ao criar o nó VarDecl.\n");
            exit(1);
        }
    }
    | TYPE VAR ASSIGN expression ENDLINE
    {
        Value *var_name = (Value *)malloc(sizeof(Value));
        if (!var_name) {
            fprintf(stderr, "Erro: Falha ao alocar memória para Value.\n");
            exit(1);
        }

        // Define o tipo da variável com base na declaração
        if (strcmp($1, "int") == 0) {
            var_name->type = TYPE_INT;
        } else if (strcmp($1, "str") == 0) {
            var_name->type = TYPE_STR;
        } else {
            fprintf(stderr, "Erro: Tipo desconhecido '%s'.\n", $1);
            exit(1);
        }

        // Copia o nome da variável
        var_name->data.str_value = strdup($2);
        if (!var_name->data.str_value) {
            fprintf(stderr, "Erro: Falha ao copiar o nome da variável.\n");
            exit(1);
        }

        // Cria o nó VarDeclAssign
        $$ = create_node("VarDeclAssign", var_name, NULL, $4);
    }
    | PREVAR VAR ASSIGN expression ENDLINE
    {
        // Cria o valor associado ao nome da variável
        Value *var_name = (Value *)malloc(sizeof(Value));
        if (!var_name) {
            fprintf(stderr, "Erro: Falha ao alocar memória para Value.\n");
            exit(1);
        }

        // Define o tipo da variável como TYPE_STR, pois é uma variável já existente
        var_name->type = TYPE_STR;
        var_name->data.str_value = strdup($2);
        if (!var_name->data.str_value) {
            fprintf(stderr, "Erro: Falha ao copiar o nome da variável.\n");
            exit(1);
        }

        // Cria o nó Var para representar a variável
        Node *var_node = create_node("Var", var_name, NULL, NULL);
        if (!var_node) {
            fprintf(stderr, "Erro: Falha ao criar o nó Var.\n");
            exit(1);
        }

        // Cria o nó de atribuição de variável
        $$ = create_node("VarAssign", NULL, var_node, $4);
        if (!$$) {
            fprintf(stderr, "Erro: Falha ao criar o nó VarAssign.\n");
            exit(1);
        }
    }
    | PRINT LPAREN expression RPAREN ENDLINE
    {
        $$ = create_node("Print", NULL, $3, NULL);
    }
    | IF LPAREN expression RPAREN block elseif_list
    {
        Node *if_node = create_node("If", NULL, $3, $5);
        if_node->next = $6;
        $$ = if_node;
    }
    | WHILE LPAREN expression RPAREN block
    {
        $$ = create_node("While", NULL, $3, $5);
    }
    ;

elseif_list:
    ELSEIF LPAREN expression RPAREN block elseif_list
    {
        Node *elseif_node = create_node("ElseIf", NULL, $3, $5);
        elseif_node->next = $6;
        $$ = elseif_node;
    }
    | ELSE block
    {
        Node *else_node = create_node("Else", NULL, NULL, $2);
        $$ = else_node;
    }
    | /* vazio */
    {
        $$ = NULL;
    }
    ;

expression:
    equality_expression
    {
        $$ = $1;
    }
    ;

equality_expression:
    equality_expression OP_EQ relational_expression
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, "=="), $1, $3);
    }
    | equality_expression OP_NE relational_expression
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, "!="), $1, $3);
    }
    | relational_expression
    {
        $$ = $1;
    }
    ;

relational_expression:
    relational_expression OP_LT additive_expression
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, "<"), $1, $3);
    }
    | relational_expression OP_GT additive_expression
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, ">"), $1, $3);
    }
    | additive_expression
    {
        $$ = $1;
    }
    ;

additive_expression:
    additive_expression OP_ADD term
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, "+"), $1, $3);
    }
    | additive_expression OP_SUB term
    {
        $$ = create_node("BinOp", create_value(TYPE_STR, "-"), $1, $3);
    }
    | term
    {
        $$ = $1;
    }
    ;

term:
    factor
    {
        $$ = $1;
    }
    ;

factor:
    NUM
    {
        Value *val = create_value(TYPE_INT, NULL);
        val->data.int_value = $1;
        $$ = create_node("IntVal", val, NULL, NULL);
    }
    | STR
    {
        Value *val = create_value(TYPE_STR, $1);
        $$ = create_node("StrVal", val, NULL, NULL);
    }
    | PREVAR VAR
    {
        Value *val = create_value(TYPE_STR, $2);
        $$ = create_node("Var", val, NULL, NULL);
    }
    | LPAREN expression RPAREN
    {
        $$ = $2;
    }
    ;

%%

