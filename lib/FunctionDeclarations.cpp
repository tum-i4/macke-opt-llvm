#include "FunctionDeclarations.h"
#include <string>
#include <vector>
#include "Arch64or32bit.h"
#include "Constants.h"
#include "llvm/IR/Type.h"


// A genric way to declare functions
llvm::Function* declare_generic(llvm::Module* Mod, const std::string name,
                                llvm::Type* returnTy,
                                std::vector<llvm::Type*> arguments) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  // Create the function, if it does not exist already
  llvm::Function* func = llvm::cast<llvm::Function>(Mod->getOrInsertFunction(
      name, llvm::FunctionType::get(
                returnTy, llvm::ArrayRef<llvm::Type*>{arguments}, false)));

  // Mark the new function as a basic c function
  func->setCallingConv(llvm::CallingConv::C);

  return func;
}

// Add declare i32 @klee_get_obj_size(i8*)
llvm::Function* declare_klee_get_obj_size(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_get_obj_size", getIntTy(Mod),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @klee_int(i8* %name)"
llvm::Function* declare_klee_int(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_int", getIntTy(Mod),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare void @klee_make_symbolic(i8*, i32, i8*)"
llvm::Function* declare_klee_make_symbolic(llvm::Module* Mod) {
  return declare_generic(
      Mod, "klee_make_symbolic", llvm::Type::getVoidTy(Mod->getContext()),
      {llvm::Type::getInt8PtrTy(Mod->getContext()), getIntTy(Mod),
       llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @klee_range(i32, i32, i8*)"
llvm::Function* declare_klee_range(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_range", getIntTy(Mod),
                         {getIntTy(Mod), getIntTy(Mod),
                          llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i8* @malloc(i32)"
llvm::Function* declare_malloc(llvm::Module* Mod) {
  return declare_generic(Mod, "malloc",
                         llvm::Type::getInt8PtrTy(Mod->getContext()),
                         {getIntTy(Mod)});
}


// Add "declare void @free(i8*)"
llvm::Function* declare_free(llvm::Module* Mod) {
  return declare_generic(Mod, "free", llvm::Type::getVoidTy(Mod->getContext()),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @memcmp(i8*, i8*, i32)"
llvm::Function* declare_memcmp(llvm::Module* Mod) {
  return declare_generic(
      Mod, "memcmp", getIntTy(Mod),
      {llvm::Type::getInt8PtrTy(Mod->getContext()),
       llvm::Type::getInt8PtrTy(Mod->getContext()), getIntTy(Mod)});
}

// Add
// ; Function Attrs: noreturn
// declare void @klee_report_error(i8*, i32, i8*, i8*)
llvm::Function* declare_klee_report_error(llvm::Module* Mod) {
  llvm::Function* func = declare_generic(
      Mod, "klee_report_error", llvm::Type::getVoidTy(Mod->getContext()),
      {llvm::Type::getInt8PtrTy(Mod->getContext()), getIntTy(Mod),
       llvm::Type::getInt8PtrTy(Mod->getContext()),
       llvm::Type::getInt8PtrTy(Mod->getContext())});

  // Add noreturn attribute
  func->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return func;
}

// Add:
// ; Function Attrs: noreturn
// declare void @klee_silent_exit(i32)
llvm::Function* declare_klee_silent_exit(llvm::Module* Mod) {
  llvm::Function* func = declare_generic(
      Mod, "klee_silent_exit", llvm::Type::getVoidTy(Mod->getContext()),
      {getIntTy(Mod)});

  // Add noreturn attribute
  func->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return func;
}
