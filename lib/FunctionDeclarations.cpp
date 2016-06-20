#include "FunctionDeclarations.h"
#include <vector>
#include "Arch64or32bit.h"

// Add "declare void @klee_make_symbolic(i8*, i32, i8*)"
llvm::Function* declare_klee_make_symbolic(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* ck = Mod->getOrInsertFunction(
      "klee_make_symbolic",
      llvm::FunctionType::get(
          modulebuilder.getVoidTy(),
          llvm::ArrayRef<llvm::Type*>{std::vector<llvm::Type*>{
              modulebuilder.getInt8Ty()->getPointerTo(), getIntTy(Mod),
              modulebuilder.getInt8Ty()->getPointerTo()}},
          false));
  llvm::Function* kleemakesym = llvm::cast<llvm::Function>(ck);
  kleemakesym->setCallingConv(llvm::CallingConv::C);

  return kleemakesym;
}


// Add "declare i8* @malloc(i32)"
llvm::Function* declare_malloc(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* cma = Mod->getOrInsertFunction(
      "malloc", llvm::FunctionType::get(
                    modulebuilder.getInt8Ty()->getPointerTo(),
                    llvm::ArrayRef<llvm::Type*>{getIntTy(Mod)}, false));
  llvm::Function* mymalloc = llvm::cast<llvm::Function>(cma);
  mymalloc->setCallingConv(llvm::CallingConv::C);

  return mymalloc;
}


// Add "declare void @free(i8*)"
llvm::Function* declare_free(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* cmf = Mod->getOrInsertFunction(
      "free",
      llvm::FunctionType::get(modulebuilder.getVoidTy(),
                              llvm::ArrayRef<llvm::Type*>{
                                  modulebuilder.getInt8Ty()->getPointerTo()},
                              false));
  llvm::Function* myfree = llvm::cast<llvm::Function>(cmf);
  myfree->setCallingConv(llvm::CallingConv::C);

  return myfree;
}
