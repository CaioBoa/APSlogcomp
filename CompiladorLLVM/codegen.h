// codegen.h

#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h>
#include "nodes.h"

extern LLVMModuleRef module;
extern LLVMBuilderRef builder;
extern LLVMContextRef context;

// Funções para gerar código LLVM IR
void codegen_program(Node *root);
LLVMValueRef codegen_node(Node *node);

#endif // CODEGEN_H
