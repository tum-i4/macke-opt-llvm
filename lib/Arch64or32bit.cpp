#include "Arch64or32bit.h"

#include "llvm/ADT/Triple.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

bool isArch64Bit(llvm::Module* Mod) {
  return llvm::Triple(Mod->getTargetTriple()).isArch64Bit();
}


llvm::Type* getIntTy(llvm::Module* Mod) {
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  return (isArch64Bit(Mod)) ? modulebuilder.getInt64Ty()
                            : modulebuilder.getInt32Ty();
}


llvm::Type* getIntTyPtr(llvm::Module* Mod) {
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  return (isArch64Bit(Mod)) ? modulebuilder.getInt64Ty()->getPointerTo()
                            : modulebuilder.getInt32Ty()->getPointerTo();
}


llvm::ConstantInt* getInt(int c, llvm::Module* Mod,
                          llvm::IRBuilder<>* builder) {
  return (isArch64Bit(Mod)) ? builder->getInt64(c) : builder->getInt32(c);
}
