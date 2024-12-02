#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Initialization.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include "parser.tab.h"
#include "symbol_table.h"
#include "nodes.h"
#include "codegen.h"

/* Variável global da tabela de símbolos */
SymbolTable *symbol_table = NULL;
int yydebug;
Node *program_root = NULL;

/* Variáveis globais para LLVM */
LLVMModuleRef module;
LLVMBuilderRef builder;
LLVMContextRef context;

int main() {
    // Ativa a depuração se necessário
    yydebug = 0;

    // Inicializa a tabela de símbolos
    symbol_table = init_symbol_table();

    // Inicializa o LLVM
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    context = LLVMContextCreate();
    module = LLVMModuleCreateWithNameInContext("my_module", context);
    builder = LLVMCreateBuilderInContext(context);

    // Cria a função 'main' e um bloco básico inicial
    LLVMTypeRef main_type = LLVMFunctionType(LLVMInt32TypeInContext(context), NULL, 0, 0);
    LLVMValueRef main_function = LLVMAddFunction(module, "main", main_type);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, main_function, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);  // Posiciona o builder

    // Parseia o programa
    if (yyparse() != 0) {
        fprintf(stderr, "Erro durante a análise sintática.\n");
        exit(1);
    }

    if (!program_root) {
        fprintf(stderr, "Erro: Árvore sintática vazia (program_root é NULL).\n");
        exit(1);
    }

    // Avalia a árvore de nós
    codegen_node(program_root);

    LLVMBuildRet(builder, LLVMConstInt(LLVMInt32TypeInContext(context), 0, 0));

    // Imprime o código LLVM no console
    char *llvm_ir = LLVMPrintModuleToString(module);
    puts(llvm_ir);
    LLVMDisposeMessage(llvm_ir);

    // Libera recursos
    free_node(program_root);
    free_symbol_table(symbol_table);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);

    return 0;
}





