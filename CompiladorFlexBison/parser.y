%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodes.h"
#include "symbol_table.h"

/* Funções auxiliares */
Node* create_node(const char *type, Value *value, Node *left, Node *right);
void free_node(Node *node);

Value* evaluate_node(Node *node);
extern Node *program_root;

/* Prototipação */
void yyerror(const char *s);
int yylex(void);

%}

%debug

%code requires {
    #include "nodes.h"
}

%union {
    int num;
    char *str;
    struct Node *node;
    Value *value;
}

/* Declaração de tokens */
%token <num> NUM
%token <str> VAR TYPE STR
%token ASSIGN OP_ADD OP_SUB OP_LT OP_GT OP_EQ OP_NE
%token PREVAR PRINT
%token LPAREN RPAREN LBRACE RBRACE
%token ENDLINE IF ELSE ELSEIF WHILE
%token START_STRING

/* Declaração de precedência dos operadores */
%left OP_EQ OP_NE
%left OP_LT OP_GT
%left OP_ADD OP_SUB

/* Declaração de tipos */
%type <node> program block statements statement expression relational_expression equality_expression additive_expression factor elseif_list

%start program

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
        $$ = $2;
    }
    ;

statements:
    statement statements
    {
        $$ = create_node("Statements", NULL, $1, $2);
    }
    |
    {
        $$ = NULL;
    }
    ;

statement:
    TYPE VAR ENDLINE
    {
        Value *var_name = (Value *)malloc(sizeof(Value));
        var_name->type = strcmp($1, "int") == 0 ? TYPE_INT : TYPE_STR;
        var_name->data.str_value = strdup($2);
        $$ = create_node("VarDecl", var_name, NULL, NULL);
    }
    | TYPE VAR ASSIGN expression ENDLINE
    {
        // Declaração de variável com atribuição
        Value *var_name = (Value *)malloc(sizeof(Value));
        var_name->type = strcmp($1, "int") == 0 ? TYPE_INT : TYPE_STR;
        var_name->data.str_value = strdup($2);
        $$ = create_node("VarDeclAssign", NULL, create_node("Var", var_name, NULL, NULL), $4);
    }
    | PREVAR VAR ASSIGN expression ENDLINE
    {
        Value *var_name = (Value *)malloc(sizeof(Value));
        var_name->type = TYPE_NONE; // Tipo será resolvido durante a avaliação
        var_name->data.str_value = strdup($2);
        $$ = create_node("VarAssign", NULL, create_node("Var", var_name, NULL, NULL), $4);
    }
    | PRINT LPAREN expression RPAREN ENDLINE
    {
        $$ = create_node("Print", NULL, $3, NULL);
    }
    | IF LPAREN expression RPAREN block elseif_list
    {
        $$ = create_node("IfElseIf", NULL, create_node("If", NULL, $3, $5), $6);
    }
    | IF LPAREN expression RPAREN block elseif_list ELSE block
    {
        Node *else_node = create_node("Else", NULL, $8, NULL);
        Node *elseif_node = create_node("IfElseIf", NULL, create_node("If", NULL, $3, $5), $6);
        $$ = create_node("IfElseIf", NULL, elseif_node, else_node);
    }
    | WHILE LPAREN expression RPAREN block
    {
        $$ = create_node("While", NULL, $3, $5);
    }
    ;

elseif_list:
    ELSEIF LPAREN expression RPAREN block
    {
        $$ = create_node("ElseIf", NULL, $3, $5);
    }
    | ELSEIF LPAREN expression RPAREN block elseif_list
    {
        Node *elseif_node = create_node("ElseIf", NULL, $3, $5);
        $$ = create_node("IfElseIf", NULL, elseif_node, $6);
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
    relational_expression
    {
        $$ = $1;
    }
    | equality_expression OP_EQ relational_expression
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup("==");
        $$ = create_node("BinOp", op, $1, $3);
    }
    | equality_expression OP_NE relational_expression
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup("!=");
        $$ = create_node("BinOp", op, $1, $3);
    }
    ;

relational_expression:
    additive_expression
    {
        $$ = $1;
    }
    | relational_expression OP_LT additive_expression
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup("<");
        $$ = create_node("BinOp", op, $1, $3);
    }
    | relational_expression OP_GT additive_expression
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup(">");
        $$ = create_node("BinOp", op, $1, $3);
    }
    ;

additive_expression:
    additive_expression OP_ADD factor
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup("+");
        $$ = create_node("BinOp", op, $1, $3);
    }
    | additive_expression OP_SUB factor
    {
        Value *op = (Value *)malloc(sizeof(Value));
        op->type = TYPE_NONE;
        op->data.str_value = strdup("-");
        $$ = create_node("BinOp", op, $1, $3);
    }
    | factor
    {
        $$ = $1;
    }
    ;

factor:
    NUM
    {
        Value *val = (Value *)malloc(sizeof(Value));
        val->type = TYPE_INT;
        val->data.int_value = $1;
        $$ = create_node("IntVal", val, NULL, NULL);
    }
    | STR
    {
        Value *val = (Value *)malloc(sizeof(Value));
        val->type = TYPE_STR;
        val->data.str_value = strdup($1);
        $$ = create_node("StrVal", val, NULL, NULL);
    }
    | PREVAR VAR
    {
        Value *var_name = (Value *)malloc(sizeof(Value));
        var_name->type = TYPE_NONE;
        var_name->data.str_value = strdup($2);
        $$ = create_node("Var", var_name, NULL, NULL);
    }
    | LPAREN expression RPAREN
    {
        $$ = $2;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
    exit(1);
}










