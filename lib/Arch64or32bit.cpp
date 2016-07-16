#include "Arch64or32bit.h"

#include "llvm/ADT/Triple.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"


bool isArch64Bit(llvm::Module* Mod) {
  return llvm::Triple(Mod->getTargetTriple()).isArch64Bit();
}


llvm::Type* getIntTy(llvm::Module* Mod) {
  return (isArch64Bit(Mod)) ? llvm::Type::getInt64Ty(Mod->getContext())
                            : llvm::Type::getInt32Ty(Mod->getContext());
}


llvm::Type* getIntTyPtr(llvm::Module* Mod) {
  return (isArch64Bit(Mod)) ? llvm::Type::getInt64PtrTy(Mod->getContext())
                            : llvm::Type::getInt32PtrTy(Mod->getContext());
}


llvm::ConstantInt* getInt(int c, llvm::Module* Mod,
                          llvm::IRBuilder<>* builder) {
  return (isArch64Bit(Mod)) ? builder->getInt64(c) : builder->getInt32(c);
}
