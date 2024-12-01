#include <stdio.h>
#include "parser.tab.h"
#include "symbol_table.h"
#include "nodes.h"

/* Variável global da tabela de símbolos */
SymbolTable *symbol_table = NULL;
extern int yydebug;
Node *program_root = NULL;

int main() {
    yydebug = 0;
    symbol_table = create_symbol_table(NULL);
    yyparse();
    evaluate_node(program_root);
    free_node(program_root);
    destroy_symbol_table(symbol_table);
    return 0;
}

