#ifndef LIB_ARCH64OR32BIT_H_
#define LIB_ARCH64OR32BIT_H_

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

llvm::Type* getIntTy(llvm::Module* Mod);
llvm::Type* getIntTyPtr(llvm::Module* Mod);
llvm::ConstantInt* getInt(int c, llvm::Module* Mod, llvm::IRBuilder<>* builder);

#endif  // LIB_ARCH64OR32BIT_H_
