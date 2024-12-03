/* codegen.c */

#include <string.h>
#include <stdbool.h>
#include "codegen.h"
#include "symbol_table.h"
#include "nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>

// Definições Globais (Assumindo que estas estão configuradas corretamente em outro lugar)
extern LLVMContextRef context;
extern LLVMModuleRef module;
extern LLVMBuilderRef builder;

// Função auxiliar para criar um novo bloco básico
LLVMBasicBlockRef create_basic_block(const char *name, LLVMValueRef function) {
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(function, name);
    return block;
}

// Função recursiva para gerar código para cada nó da AST
LLVMValueRef codegen_node(Node *node, CodegenContext *ctx) {
    if (!node) {
        fprintf(stderr, "Erro: node é NULL em codegen_node.\n");
        exit(1);
    }

    // Verifica se o builder está em um bloco básico válido
    if (!LLVMGetInsertBlock(builder)) {
        fprintf(stderr, "Erro: LLVMBuilder não posicionado em um bloco válido.\n");
        exit(1);
    }

    // Obtém a função atual a partir do bloco básico inserido
    LLVMValueRef function = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));

    if (strcmp(node->type, "Statements") == 0) {
        if (node->left) {
            codegen_node(node->left, ctx);
        }
        if (node->right) {
            codegen_node(node->right, ctx);
        }
    }
    else if (strcmp(node->type, "Block") == 0) {
        if (node->left) {
            codegen_node(node->left, ctx);
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
    else if (strcmp(node->type, "VarDeclAssign") == 0) {
        if (!node->value || !node->value->data.str_value) {
            fprintf(stderr, "Erro: 'VarDeclAssign' com valor nulo.\n");
            exit(1);
        }

        // Determina o tipo correto
        LLVMTypeRef llvm_type = (node->value->type == TYPE_INT)
                                    ? LLVMInt32TypeInContext(context)
                                    : LLVMPointerType(LLVMInt8TypeInContext(context), 0);

        // Cria a alocação para a variável
        LLVMValueRef var_alloca = LLVMBuildAlloca(builder, llvm_type, node->value->data.str_value);

        // Obter o valor atribuído
        LLVMValueRef init_value = codegen_node(node->right, ctx);
        ValueType assign_type = (LLVMGetTypeKind(LLVMTypeOf(init_value)) == LLVMPointerTypeKind)
                                    ? TYPE_STR
                                    : TYPE_INT;

        // Verificar compatibilidade de tipos
        if (node->value->type != assign_type) {
            fprintf(stderr, "Erro: Tipo incompatível em declaração de variável '%s'. Esperado '%s', mas recebeu '%s'.\n",
                    node->value->data.str_value,
                    node->value->type == TYPE_INT ? "int" : "string",
                    assign_type == TYPE_INT ? "int" : "string");
            exit(1);
        }

        // Inicializa a variável e insere na tabela de símbolos
        LLVMBuildStore(builder, init_value, var_alloca);
        insert_symbol(node->value->data.str_value, var_alloca, node->value->type);
    }
    else if (strcmp(node->type, "VarAssign") == 0) {
        if (!node->left || !node->left->value) {
            fprintf(stderr, "Erro: VarAssign com nó ou valor nulo.\n");
            exit(1);
        }

        // Busca a variável na tabela de símbolos
        Symbol *entry = lookup_symbol(node->left->value->data.str_value);
        if (!entry) {
            fprintf(stderr, "Erro: variável '%s' não declarada.\n", node->left->value->data.str_value);
            exit(1);
        }

        // Determina o tipo da variável
        ValueType var_type = entry->type;

        // Obter o valor atribuído
        LLVMValueRef assign_value = codegen_node(node->right, ctx);
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
        LLVMBuildStore(builder, assign_value, entry->value);
    }
    else if (strcmp(node->type, "Var") == 0) {
        if (node->value && node->value->data.str_value) {
            // Busca a variável na tabela de símbolos
            Symbol *entry = lookup_symbol(node->value->data.str_value);
            if (!entry) {
                fprintf(stderr, "Erro: variável '%s' não encontrada.\n", node->value->data.str_value);
                exit(1);
            }

            // Determina o tipo associado à variável
            LLVMTypeRef llvm_type = (entry->type == TYPE_INT)
                                        ? LLVMInt32TypeInContext(context)
                                        : LLVMPointerType(LLVMInt8TypeInContext(context), 0);

            // Carrega o valor da variável
            return LLVMBuildLoad2(builder, llvm_type, entry->value, node->value->data.str_value);
        } else {
            fprintf(stderr, "Erro: 'Var' com valor nulo.\n");
            exit(1);
        }
    }
    else if (strcmp(node->type, "Print") == 0) {
        // Gera o código para o valor a ser impresso
        LLVMValueRef value = codegen_node(node->left, ctx);

        static LLVMValueRef printf_func = NULL;
        if (!printf_func) {
            // Define a função printf se ainda não foi definida
            LLVMTypeRef printf_arg_types[] = { LLVMPointerType(LLVMInt8TypeInContext(context), 0) };
            LLVMTypeRef printf_type = LLVMFunctionType(
                LLVMInt32TypeInContext(context), printf_arg_types, 1, true);
            printf_func = LLVMAddFunction(module, "printf", printf_type);
        }

        LLVMValueRef format_str = NULL;
        LLVMValueRef args[2];

        // Determina o tipo do valor usando LLVMTypeKind
        LLVMTypeKind type_kind = LLVMGetTypeKind(LLVMTypeOf(value));

        if (type_kind == LLVMPointerTypeKind) {
            // Caso seja uma string
            format_str = LLVMBuildGlobalStringPtr(builder, "%s\n", "fmt");
            args[0] = format_str;
            args[1] = value;
            LLVMBuildCall2(
                builder,
                LLVMGetElementType(LLVMTypeOf(printf_func)),
                printf_func,
                args,
                2, // Dois argumentos: formato e valor
                "printf_call"
            );
        }
        else if (type_kind == LLVMIntegerTypeKind && LLVMGetIntTypeWidth(LLVMTypeOf(value)) == 32) {
            // Caso seja um inteiro de 32 bits
            format_str = LLVMBuildGlobalStringPtr(builder, "%d\n", "fmt");
            args[0] = format_str;
            args[1] = value;
            LLVMBuildCall2(
                builder,
                LLVMGetElementType(LLVMTypeOf(printf_func)),
                printf_func,
                args,
                2, // Dois argumentos: formato e valor
                "printf_call"
            );
        }
        else if (type_kind == LLVMIntegerTypeKind && LLVMGetIntTypeWidth(LLVMTypeOf(value)) == 1) {
            // Caso seja um booleano (i1)
            // Converte de i1 para i32 usando extensão zero
            LLVMValueRef extended_val = LLVMBuildZExt(builder, value, LLVMInt32TypeInContext(context), "bool_ext");
            // Usa o formato de inteiro para imprimir 1 ou 0
            format_str = LLVMBuildGlobalStringPtr(builder, "%d\n", "fmt");
            args[0] = format_str;
            args[1] = extended_val;
            LLVMBuildCall2(
                builder,
                LLVMGetElementType(LLVMTypeOf(printf_func)),
                printf_func,
                args,
                2, // Dois argumentos: formato e valor convertido
                "printf_call"
            );
        }
        else {
            // Tipo de valor não suportado para impressão
            fprintf(stderr, "Erro: Tipo de valor não suportado para print.\n");
            exit(1); // Termina a geração de código para evitar LLVM IR inválido
        }
    }
    else if (strcmp(node->type, "BinOp") == 0) {
        // Gerar código para os operandos esquerdo e direito
        LLVMValueRef left = codegen_node(node->left, ctx);
        LLVMValueRef right = codegen_node(node->right, ctx);

        // Obter os tipos dos operandos
        LLVMTypeRef left_type = LLVMTypeOf(left);
        LLVMTypeRef right_type = LLVMTypeOf(right);

        // Obter o operador
        const char* op = node->value->data.str_value;

        // Definir as regras de compatibilidade de tipos para cada operador
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) {
            // Operadores aritméticos requerem ambos os operandos como int32
            if (LLVMGetTypeKind(left_type) != LLVMIntegerTypeKind || 
                LLVMGetTypeKind(right_type) != LLVMIntegerTypeKind ||
                LLVMGetIntTypeWidth(left_type) != 32 || 
                LLVMGetIntTypeWidth(right_type) != 32) {
                fprintf(stderr, "Erro: Operação '%s' requer ambos os operandos como 'int'.\n", op);
                exit(1); // Termina a geração de código
            }

            // Gerar a instrução LLVM apropriada
            if (strcmp(op, "+") == 0) {
                return LLVMBuildAdd(builder, left, right, "addtmp");
            } else { // "-"
                return LLVMBuildSub(builder, left, right, "subtmp");
            }
        }
        else if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
                strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            // Operadores de comparação podem ser usados com int32 ou strings (i8*)
            bool is_int_comparison = (LLVMGetTypeKind(left_type) == LLVMIntegerTypeKind &&
                                    LLVMGetTypeKind(right_type) == LLVMIntegerTypeKind &&
                                    LLVMGetIntTypeWidth(left_type) == 32 &&
                                    LLVMGetIntTypeWidth(right_type) == 32);

            bool is_string_comparison = (LLVMGetTypeKind(left_type) == LLVMPointerTypeKind &&
                                        LLVMGetTypeKind(right_type) == LLVMPointerTypeKind &&
                                        LLVMGetElementType(left_type) == LLVMInt8TypeInContext(context) &&
                                        LLVMGetElementType(right_type) == LLVMInt8TypeInContext(context));

            if (!is_int_comparison && !is_string_comparison) {
                fprintf(stderr, "Erro: Operação '%s' requer ambos os operandos como 'int' ou 'string'.\n", op);
                exit(1); // Termina a geração de código
            }

            // Gerar a instrução de comparação apropriada
            if (strcmp(op, "<") == 0) {
                if (is_int_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntSLT, left, right, "cmptmp");
                } else {
                    fprintf(stderr, "Erro: Operação '%s' não suportada para 'string'.\n", op);
                    exit(1);
                }
            }
            else if (strcmp(op, ">") == 0) {
                if (is_int_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntSGT, left, right, "cmptmp");
                } else {
                    fprintf(stderr, "Erro: Operação '%s' não suportada para 'string'.\n", op);
                    exit(1);
                }
            }
            else if (strcmp(op, "==") == 0) {
                if (is_int_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntEQ, left, right, "cmptmp");
                } else if (is_string_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntEQ, left, right, "cmptmp");
                }
            }
            else if (strcmp(op, "!=") == 0) {
                if (is_int_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntNE, left, right, "cmptmp");
                } else if (is_string_comparison) {
                    return LLVMBuildICmp(builder, LLVMIntNE, left, right, "cmptmp");
                }
            }
        }
        else {
            fprintf(stderr, "Erro: Operador binário não suportado: '%s'.\n", op);
            exit(1); // Termina a geração de código
        }

        // Se chegar aqui, significa que o operador não foi tratado
        fprintf(stderr, "Erro: Operador binário '%s' não tratado corretamente.\n", op);
        exit(1);
    }
    else if (strcmp(node->type, "While") == 0) {
        // Criação dos blocos básicos para o loop
        LLVMBasicBlockRef loopcond = LLVMAppendBasicBlock(function, "loopcond");
        LLVMBasicBlockRef loopbody = LLVMAppendBasicBlock(function, "loopbody");
        LLVMBasicBlockRef afterloop = LLVMAppendBasicBlock(function, "afterloop");

        // Salta para o bloco de condição
        LLVMBuildBr(builder, loopcond);

        // Salva os blocos no contexto para uso dentro do loop
        // (Assumindo que CodegenContext possui esses campos)
        // ctx->loopcond = loopcond;
        // ctx->loopbody = loopbody;
        // ctx->afterloop = afterloop;

        // Bloco de condição
        LLVMPositionBuilderAtEnd(builder, loopcond);
        LLVMValueRef cond = codegen_node(node->left, ctx);
        cond = LLVMBuildIntCast(builder, cond, LLVMInt1TypeInContext(context), "booltmp");
        LLVMBuildCondBr(builder, cond, loopbody, afterloop);

        // Bloco do corpo do loop
        LLVMPositionBuilderAtEnd(builder, loopbody);
        codegen_node(node->right, ctx);
        LLVMBuildBr(builder, loopcond);

        // Bloco após o loop
        LLVMPositionBuilderAtEnd(builder, afterloop);
    }
    else if (strcmp(node->type, "If") == 0) {
        // Tipo do nó: "If"
        // node->Left = condition (expression)
        // node->Right = then_block (block)
        // node->Next = elseif_list (ElseIf) ou Else

        // Avaliar a condição
        LLVMValueRef cond = codegen_node(node->left, ctx);
        cond = LLVMBuildIntCast(builder, cond, LLVMInt1TypeInContext(context), "ifcond");

        // Criar blocos básicos para then, else e continuação
        LLVMBasicBlockRef then_block = create_basic_block("then", function);
        LLVMBasicBlockRef else_block = create_basic_block("else", function);
        LLVMBasicBlockRef merge_block = create_basic_block("ifcont", function);

        // Atualizar o contexto com o merge_block
        LLVMBasicBlockRef previous_merge = ctx->merge_block;
        ctx->merge_block = merge_block;

        // Branching condicional para then ou else
        LLVMBuildCondBr(builder, cond, then_block, else_block);

        // Bloco "then"
        LLVMPositionBuilderAtEnd(builder, then_block);
        codegen_node(node->right, ctx); // Executa o bloco 'then'
        // Salta para o bloco de continuação
        LLVMBuildBr(builder, merge_block);

        // Bloco "else"
        LLVMPositionBuilderAtEnd(builder, else_block);
        if (node->next) {
            if (strcmp(node->next->type, "ElseIf") == 0) {
                // Definir ctx->merge_block para merge_block
                LLVMBasicBlockRef previous_afterloop = ctx->merge_block;
                ctx->merge_block = merge_block;
                codegen_node(node->next, ctx);
                // Restaura ctx->merge_block
                ctx->merge_block = previous_afterloop;
            }
            else if (strcmp(node->next->type, "Else") == 0) {
                // Definir ctx->merge_block para merge_block
                LLVMBasicBlockRef previous_afterloop = ctx->merge_block;
                ctx->merge_block = merge_block;
                codegen_node(node->next, ctx);
                // Restaura ctx->merge_block
                ctx->merge_block = previous_afterloop;
            }
            else {
                // Nenhum 'ElseIf' ou 'Else' adicional
                // Salta para o bloco de continuação
                LLVMBuildBr(builder, merge_block);
            }
        }
        else {
            // Nenhum 'ElseIf' ou 'Else' adicional
            // Salta para o bloco de continuação
            LLVMBuildBr(builder, merge_block);
        }

        // Bloco de continuação
        LLVMPositionBuilderAtEnd(builder, merge_block);
        // Restaurar o merge_block anterior
        ctx->merge_block = previous_merge;
    }
    else if (strcmp(node->type, "ElseIf") == 0) {
        // Nó "ElseIf" possui:
        // node->Left = condition (expression)
        // node->Right = then_block (block)
        // node->Next = próximo ElseIf ou Else

        // Avaliar a condição
        LLVMValueRef cond = codegen_node(node->left, ctx);
        cond = LLVMBuildIntCast(builder, cond, LLVMInt1TypeInContext(context), "elseifcond");

        // Criar blocos básicos para then, else e continuação
        LLVMBasicBlockRef elseif_then = create_basic_block("elseif_then", function);
        LLVMBasicBlockRef elseif_else = create_basic_block("elseif_else", function);
        LLVMBasicBlockRef elseif_cont = create_basic_block("elseif_cont", function);

        // Branching condicional para then ou else
        LLVMBuildCondBr(builder, cond, elseif_then, elseif_else);

        // Bloco "then" do ElseIf
        LLVMPositionBuilderAtEnd(builder, elseif_then);
        codegen_node(node->right, ctx); // Executa o bloco 'then' do ElseIf
        // Salta para o bloco de continuação
        LLVMBuildBr(builder, ctx->merge_block); // Direciona para 'ifcont'

        // Bloco "else" do ElseIf
        LLVMPositionBuilderAtEnd(builder, elseif_else);
        if (node->next) {
            if (strcmp(node->next->type, "ElseIf") == 0) {
                // Definir ctx->merge_block para merge_block
                LLVMBasicBlockRef previous_merge = ctx->merge_block;
                codegen_node(node->next, ctx);
                // Restaurar ctx->merge_block
                ctx->merge_block = previous_merge;
            }
            else if (strcmp(node->next->type, "Else") == 0) {
                // Definir ctx->merge_block para merge_block
                LLVMBasicBlockRef previous_merge = ctx->merge_block;
                codegen_node(node->next, ctx);
                // Restaurar ctx->merge_block
                ctx->merge_block = previous_merge;
            }
            else {
                // Nenhum 'ElseIf' ou 'Else' adicional
                // Salta para o bloco de continuação
                LLVMBuildBr(builder, ctx->merge_block);
            }
        }
        else {
            // Nenhum 'ElseIf' ou 'Else' adicional
            // Salta para o bloco de continuação
            LLVMBuildBr(builder, ctx->merge_block);
        }

        // Bloco de continuação
        LLVMPositionBuilderAtEnd(builder, elseif_cont);
        LLVMBuildBr(builder, ctx->merge_block); // Direciona para 'ifcont'
    }
    else if (strcmp(node->type, "Else") == 0) {
        // Nó "Else" possui:
        // node->Left = bloco 'else'
        // node->Right = NULL

        // Gerar código para o bloco 'else'
        codegen_node(node->right, ctx); // Alterado de node->right para node->left
        // Após o bloco 'else', salta para o merge_block
        if (ctx->merge_block) { // Alterado de ctx->afterloop para ctx->merge_block
            LLVMBuildBr(builder, ctx->merge_block); // Alterado de ctx->afterloop para ctx->merge_block
        } else {
            fprintf(stderr, "Erro: ctx->merge_block não está definido para 'Else'.\n");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Erro : Tipo de nó não suportado: %s\n", node->type);
        exit(1);
    }

    return NULL;
}




