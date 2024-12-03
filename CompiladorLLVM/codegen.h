// codegen.h

#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h>
#include "nodes.h"

typedef struct CodegenContext {
    LLVMBasicBlockRef merge_block; // Bloco de continuação comum
} CodegenContext;

extern LLVMModuleRef module;
extern LLVMBuilderRef builder;
extern LLVMContextRef context;

// Funções para gerar código LLVM IR
LLVMValueRef codegen_node(Node *node, CodegenContext *ctx);

#endif // CODEGEN_H
