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

// Add "declare i64 @klee_get_obj_size(i8*)"
// 32: "declare i32 @klee_get_obj_size(i8*)"
llvm::Function* declare_klee_get_obj_size(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_get_obj_size", getIntTy(Mod),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @klee_int(i8* %name)"
// 32: "declare i32 @klee_int(i8* %name)"
llvm::Function* declare_klee_int(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_int",
                         llvm::Type::getInt32Ty(Mod->getContext()),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare void @klee_make_symbolic(i8*, i64, i8*)"
// 32: "declare void @klee_make_symbolic(i8*, i32, i8*)"
llvm::Function* declare_klee_make_symbolic(llvm::Module* Mod) {
  return declare_generic(
      Mod, "klee_make_symbolic", llvm::Type::getVoidTy(Mod->getContext()),
      {llvm::Type::getInt8PtrTy(Mod->getContext()), getIntTy(Mod),
       llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @klee_range(i32, i32, i8*)"
// 32: "declare i32 @klee_range(i32, i32, i8*)"
llvm::Function* declare_klee_range(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_range",
                         llvm::Type::getInt32Ty(Mod->getContext()),
                         {llvm::Type::getInt32Ty(Mod->getContext()),
                          llvm::Type::getInt32Ty(Mod->getContext()),
                          llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare noalias i8* @malloc(i64)"
// 32: "declare noalias i8* @malloc(i32)"
llvm::Function* declare_malloc(llvm::Module* Mod) {
  llvm::Function* func = declare_generic(
      Mod, "malloc", llvm::Type::getInt8PtrTy(Mod->getContext()),
      {getIntTy(Mod)});

  // Add noalias attribute
  func->setDoesNotAlias(0);

  return func;
}

// Add "declare void @free(i8*)"
// 32: "declare void @free(i8*)"
llvm::Function* declare_free(llvm::Module* Mod) {
  return declare_generic(Mod, "free", llvm::Type::getVoidTy(Mod->getContext()),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add "declare i32 @memcmp(i8*, i8*, i64)"
// 32: "declare i32 @memcmp(i8*, i8*, i32)"
llvm::Function* declare_memcmp(llvm::Module* Mod) {
  return declare_generic(
      Mod, "memcmp", llvm::Type::getInt32Ty(Mod->getContext()),
      {llvm::Type::getInt8PtrTy(Mod->getContext()),
       llvm::Type::getInt8PtrTy(Mod->getContext()), getIntTy(Mod)});
}

// Add
// ; Function Attrs: noreturn
// 64: "declare void @klee_report_error(i8*, i32, i8*, i8*)"
// 32: "declare void @klee_report_error(i8*, i32, i8*, i8*)"
llvm::Function* declare_klee_report_error(llvm::Module* Mod) {
  llvm::Function* func = declare_generic(
      Mod, "klee_report_error", llvm::Type::getVoidTy(Mod->getContext()),
      {llvm::Type::getInt8PtrTy(Mod->getContext()),
       llvm::Type::getInt32Ty(Mod->getContext()),
       llvm::Type::getInt8PtrTy(Mod->getContext()),
       llvm::Type::getInt8PtrTy(Mod->getContext())});

  // Add noreturn attribute
  func->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return func;
}

// Add:
// ; Function Attrs: noreturn
// 64: "declare void @klee_silent_exit(i32)"
// 32: "declare void @klee_silent_exit(i32)"
llvm::Function* declare_klee_silent_exit(llvm::Module* Mod) {
  llvm::Function* func = declare_generic(
      Mod, "klee_silent_exit", llvm::Type::getVoidTy(Mod->getContext()),
      {llvm::Type::getInt32Ty(Mod->getContext())});

  // Add noreturn attribute
  func->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return func;
}

// Add:
// 64: "declare void @klee_warning(i8*)"
// 32: "declare void @klee_warning(i8*)"
llvm::Function* declare_klee_warning(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_warning",
                         llvm::Type::getVoidTy(Mod->getContext()),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}

// Add:
// 64: "declare void @klee_warning_once(i8*)"
// 32: "declare void @klee_warning_once(i8*)"
llvm::Function* declare_klee_warning_once(llvm::Module* Mod) {
  return declare_generic(Mod, "klee_warning_once",
                         llvm::Type::getVoidTy(Mod->getContext()),
                         {llvm::Type::getInt8PtrTy(Mod->getContext())});
}
