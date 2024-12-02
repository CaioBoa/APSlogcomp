#include <string.h>
#include <stdbool.h> // Adicionado
#include "codegen.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SYMBOLS 100
int symbol_count = 0;

// Função para adicionar uma variável à tabela de símbolos
void add_symbol(char *name, LLVMValueRef value) {
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Erro: número máximo de símbolos excedido.\n");
        exit(1);
    }
    insert_symbol(name, value);
    symbol_count++;
}

// Função recursiva para gerar código para cada nó
LLVMValueRef codegen_node(Node *node) {
    if (!node) {
        fprintf(stderr, "Erro: node é NULL em codegen_node.\n");
        return NULL;
    }

    if (!LLVMGetInsertBlock(builder)) {
        fprintf(stderr, "Erro: LLVMBuilder não posicionado em um bloco válido.\n");
        exit(1);
    }

    if (strcmp(node->type, "Statements") == 0) {
        if (node->left) {
            codegen_node(node->left);
        }
        if (node->right) {
            codegen_node(node->right);
        }
    }
    else if (strcmp(node->type, "Block") == 0) {
        if (node->left) {
            codegen_node(node->left);
        }
    }
    else if (strcmp(node->type, "IfElseIf") == 0) {
        codegen_node(node->left); // Processa o nó 'If' ou 'ElseIf'
        if (node->right) {
            codegen_node(node->right); // Processa o próximo 'ElseIf' ou 'Else'
        }
    }
    else if (strcmp(node->type, "IntVal") == 0) {
        if (!node->value) {
            fprintf(stderr, "Erro: IntVal com valor nulo.\n");
            exit(1);
        }
        return LLVMConstInt(LLVMInt32TypeInContext(context), node->value->data.int_value, 0);
    }
    else if (strcmp(node->type, "StrVal") == 0) {
        if (!node->value || !node->value->data.str_value) {
            fprintf(stderr, "Erro: StrVal com valor nulo.\n");
            exit(1);
        }
        // Cria um ponteiro para a string no módulo LLVM
        return LLVMBuildGlobalStringPtr(builder, node->value->data.str_value, "strtmp");
    }
    else if (strcmp(node->type, "VarDecl") == 0) {
        LLVMTypeRef var_type = node->left->value->type == TYPE_STR
            ? LLVMPointerType(LLVMInt8TypeInContext(context), 0) // Ponteiro para string
            : LLVMInt32TypeInContext(context);                  // Inteiro

        LLVMValueRef var_alloca = LLVMBuildAlloca(
            builder,
            var_type,
            node->left->value->data.str_value
        );
        add_symbol(node->left->value->data.str_value, var_alloca);
    }
    else if (strcmp(node->type, "VarDeclAssign") == 0) {
        if (!node->value || !node->value->data.str_value) {
            fprintf(stderr, "Erro: VarDeclAssign com valor ou nome nulo.\n");
            exit(1);
        }

        // Tipo declarado da variável
        ValueType declared_type = node->value->type;

        // Gerar o valor inicial da variável
        LLVMValueRef init_value = codegen_node(node->right);
        ValueType init_type = (LLVMGetTypeKind(LLVMTypeOf(init_value)) == LLVMPointerTypeKind)
                                ? TYPE_STR
                                : TYPE_INT;

        // Verificar compatibilidade de tipos
        if (declared_type != init_type) {
            fprintf(stderr, "Erro: Tentativa de inicializar variável '%s' de tipo '%s' com valor de tipo '%s'.\n",
                    node->value->data.str_value,
                    declared_type == TYPE_INT ? "int" : "string",
                    init_type == TYPE_INT ? "int" : "string");
            exit(1);
        }

        // Insere a variável na tabela de símbolos
        LLVMValueRef var_alloca = LLVMBuildAlloca(builder, 
            declared_type == TYPE_INT ? LLVMInt32TypeInContext(context) : LLVMPointerType(LLVMInt8TypeInContext(context), 0),
            node->value->data.str_value);
        LLVMBuildStore(builder, init_value, var_alloca);

        insert_symbol(node->value->data.str_value, var_alloca);
    }
    else if (strcmp(node->type, "VarAssign") == 0) {
        if (!node->left || !node->left->value) {
            fprintf(stderr, "Erro: VarAssign com nó ou valor nulo.\n");
            exit(1);
        }

        // Busca a variável na tabela de símbolos
        LLVMValueRef var_alloca = lookup_symbol(node->left->value->data.str_value);

        if (!var_alloca) {
            fprintf(stderr, "Erro: variável '%s' não declarada.\n", node->left->value->data.str_value);
            exit(1);
        }

        // Obter o tipo da variável
        ValueType var_type = node->left->value->type;

        // Obter o tipo do valor atribuído
        LLVMValueRef assign_value = codegen_node(node->right);
        ValueType assign_type = (LLVMGetTypeKind(LLVMTypeOf(assign_value)) == LLVMPointerTypeKind)
                                    ? TYPE_STR
                                    : TYPE_INT;

        // Verificar compatibilidade de tipos
        if (var_type != assign_type) {
            fprintf(stderr, "Erro: Tentativa de atribuir valor de tipo '%s' a variável '%s' de tipo '%s'.\n",
                    assign_type == TYPE_INT ? "int" : "string",
                    node->left->value->data.str_value,
                    var_type == TYPE_INT ? "int" : "string");
            exit(1);
        }

        // Realiza a atribuição
        LLVMBuildStore(builder, assign_value, var_alloca);
    }
    else if (strcmp(node->type, "Var") == 0) {
        if (node->value && node->value->data.str_value) {
            // Busca a variável na tabela de símbolos
            LLVMValueRef var_alloca = lookup_symbol(node->value->data.str_value);
            if (!var_alloca) {
                fprintf(stderr, "Erro: variável '%s' não encontrada.\n", node->value->data.str_value);
                exit(1);
            }

            // Determina o tipo real da variável com base no LLVMTypeRef do var_alloca
            LLVMTypeRef var_alloca_type = LLVMGetElementType(LLVMTypeOf(var_alloca));
            LLVMTypeKind type_kind = LLVMGetTypeKind(var_alloca_type);

            LLVMTypeRef var_type;
            if (type_kind == LLVMIntegerTypeKind) {
                var_type = LLVMInt32TypeInContext(context); // Variável do tipo int
            } else if (type_kind == LLVMPointerTypeKind) {
                var_type = LLVMPointerType(LLVMInt8TypeInContext(context), 0); // Variável do tipo string
            } else {
                fprintf(stderr, "Erro: tipo desconhecido para a variável '%s'.\n", node->value->data.str_value);
                exit(1);
            }

            // Retorna a carga do valor com o tipo correto
            return LLVMBuildLoad2(builder, var_type, var_alloca, node->value->data.str_value);
        } else {
            fprintf(stderr, "Erro: 'Var' com valor nulo.\n");
            exit(1);
        }
    }
    else if (strcmp(node->type, "Print") == 0) {
        LLVMValueRef value = codegen_node(node->left);

        static LLVMValueRef printf_func = NULL;
        if (!printf_func) {
            LLVMTypeRef printf_args_type[] = { LLVMPointerType(LLVMInt8TypeInContext(context), 0) };
            LLVMTypeRef printf_type = LLVMFunctionType(
                LLVMInt32TypeInContext(context), printf_args_type, 1, true);
            printf_func = LLVMAddFunction(module, "printf", printf_type);
        }

        LLVMValueRef format_str = NULL;
        LLVMValueRef args[2];

        // Determina se o valor é string ou int
        if (LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMPointerTypeKind) {
            // Formato para strings
            format_str = LLVMBuildGlobalStringPtr(builder, "%s\n", "fmt");
            args[0] = format_str;
            args[1] = value;
            LLVMBuildCall2(
                builder,
                LLVMGetElementType(LLVMTypeOf(printf_func)),
                printf_func,
                args,
                2, // Dois argumentos para strings
                "printf_call"
            );
        } else {
            // Formato para inteiros
            format_str = LLVMBuildGlobalStringPtr(builder, "%d\n", "fmt");
            args[0] = format_str;
            args[1] = value;
            LLVMBuildCall2(
                builder,
                LLVMGetElementType(LLVMTypeOf(printf_func)),
                printf_func,
                args,
                2, // Dois argumentos para inteiros
                "printf_call"
            );
        }
    }
    else if (strcmp(node->type, "BinOp") == 0) {
        LLVMValueRef left = codegen_node(node->left);
        LLVMValueRef right = codegen_node(node->right);

        if (node->value && node->value->data.str_value) {
            if (strcmp(node->value->data.str_value, "+") == 0) {
                return LLVMBuildAdd(builder, left, right, "addtmp");
            } else if (strcmp(node->value->data.str_value, "-") == 0) {
                return LLVMBuildSub(builder, left, right, "subtmp");
            } else if (strcmp(node->value->data.str_value, "<") == 0) {
                return LLVMBuildICmp(builder, LLVMIntSLT, left, right, "cmptmp");
            } else if (strcmp(node->value->data.str_value, ">") == 0) {
                return LLVMBuildICmp(builder, LLVMIntSGT, left, right, "cmptmp");
            } else {
                fprintf(stderr, "Operador binário não suportado: %s\n", node->value->data.str_value);
                exit(1);
            }
        } else {
            fprintf(stderr, "Erro: 'BinOp' com valor nulo.\n");
            exit(1);
        }
    }
    else if (strcmp(node->type, "If") == 0) {
        // Condição
        LLVMValueRef cond = codegen_node(node->left);
        cond = LLVMBuildIntCast(builder, cond, LLVMInt1TypeInContext(context), "booltmp");

        // Criar blocos
        LLVMValueRef function = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
        LLVMBasicBlockRef then_block = LLVMAppendBasicBlockInContext(context, function, "then");
        LLVMBasicBlockRef merge_block = LLVMAppendBasicBlockInContext(context, function, "ifcont");

        LLVMBuildCondBr(builder, cond, then_block, merge_block);

        // Bloco 'then'
        LLVMPositionBuilderAtEnd(builder, then_block);
        codegen_node(node->right); // Processar o bloco do 'If'
        LLVMBuildBr(builder, merge_block);

        // Bloco de mesclagem
        LLVMPositionBuilderAtEnd(builder, merge_block);
    }

    else if (strcmp(node->type, "Else") == 0) {
        codegen_node(node->left); // Processa o bloco do 'Else'
    }
    else if (strcmp(node->type, "While") == 0) {
        LLVMValueRef function = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
        LLVMBasicBlockRef cond_block = LLVMAppendBasicBlockInContext(context, function, "loopcond");
        LLVMBasicBlockRef body_block = LLVMAppendBasicBlockInContext(context, function, "loopbody");
        LLVMBasicBlockRef after_block = LLVMAppendBasicBlockInContext(context, function, "afterloop");

        LLVMBuildBr(builder, cond_block);

        // Bloco de condição
        LLVMPositionBuilderAtEnd(builder, cond_block);
        LLVMValueRef cond = codegen_node(node->left);
        cond = LLVMBuildIntCast(builder, cond, LLVMInt1TypeInContext(context), "booltmp");
        LLVMBuildCondBr(builder, cond, body_block, after_block);

        // Bloco do corpo do loop
        LLVMPositionBuilderAtEnd(builder, body_block);
        codegen_node(node->right);
        LLVMBuildBr(builder, cond_block);

        // Bloco após o loop
        LLVMPositionBuilderAtEnd(builder, after_block);
    }

    // Implementar outros tipos de nós conforme necessário
    else {
        fprintf(stderr, "Tipo de nó não suportado: %s\n", node->type);
        exit(1);
    }

    return NULL;
}
