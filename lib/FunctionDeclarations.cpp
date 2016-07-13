#include "FunctionDeclarations.h"
#include <vector>
#include "Arch64or32bit.h"


// Add "declare i32 @klee_int(i8* %name)"
llvm::Function* declare_klee_int(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* ck = Mod->getOrInsertFunction(
      "klee_int",
      llvm::FunctionType::get(getIntTy(Mod),
                              llvm::ArrayRef<llvm::Type*>{
                                  modulebuilder.getInt8Ty()->getPointerTo()},
                              false));
  llvm::Function* kleeint = llvm::cast<llvm::Function>(ck);
  kleeint->setCallingConv(llvm::CallingConv::C);

  return kleeint;
}


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


// Add "declare i32 @klee_range(i32, i32, i8*)"
llvm::Function* declare_klee_range(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* ck = Mod->getOrInsertFunction(
      "klee_range",
      llvm::FunctionType::get(
          getIntTy(Mod), llvm::ArrayRef<llvm::Type*>{std::vector<llvm::Type*>{
                             getIntTy(Mod), getIntTy(Mod),
                             modulebuilder.getInt8Ty()->getPointerTo()}},
          false));
  llvm::Function* kleerange = llvm::cast<llvm::Function>(ck);
  kleerange->setCallingConv(llvm::CallingConv::C);

  return kleerange;
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


// Add
// ; Function Attrs: noreturn
// declare void @klee_report_error(i8*, i32, i8*, i8*)
llvm::Function* declare_klee_report_error(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* ckre = Mod->getOrInsertFunction(
      "klee_report_error",
      llvm::FunctionType::get(
          modulebuilder.getVoidTy(),
          llvm::ArrayRef<llvm::Type*>{std::vector<llvm::Type*>{
              modulebuilder.getInt8Ty()->getPointerTo(), getIntTy(Mod),
              modulebuilder.getInt8Ty()->getPointerTo(),
              modulebuilder.getInt8Ty()->getPointerTo()}},
          false));
  llvm::Function* kleereporterror = llvm::cast<llvm::Function>(ckre);
  kleereporterror->setCallingConv(llvm::CallingConv::C);

  // Add noreturn attribute
  kleereporterror->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return kleereporterror;
}


// Add:
// ; Function Attrs: noreturn
// declare void @klee_silent_exit(i32)
llvm::Function* declare_klee_silent_exit(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  llvm::Constant* cms = Mod->getOrInsertFunction(
      "klee_silent_exit",
      llvm::FunctionType::get(modulebuilder.getVoidTy(),
                              llvm::ArrayRef<llvm::Type*>{getIntTy(Mod)},
                              false));
  llvm::Function* mysilent = llvm::cast<llvm::Function>(cms);
  mysilent->setCallingConv(llvm::CallingConv::C);

  // Add noreturn attribute
  mysilent->addFnAttr(llvm::Attribute::AttrKind::NoReturn);

  return mysilent;
}


// Create a function equivalent to
// size_t foo(int n) {
//     switch(n) {
//         default:
//         case   1: return   1; break;
//         case   2: return   2; break;
//         case   4: return   4; break;
//         case  16: return  16; break;
//         case 256: return 256; break;
//     }
// }
// Add define "i32 @macke_fork_several_sizes(i32 %n)""
llvm::Function* define_macke_fork_several_sizes(llvm::Module* Mod) {
  // Create a builder for this module
  llvm::IRBuilder<> modulebuilder(Mod->getContext());

  // Create function declaration
  llvm::Constant* ck = Mod->getOrInsertFunction(
      "macke_fork_several_sizes",
      llvm::FunctionType::get(
          getIntTy(Mod), llvm::ArrayRef<llvm::Type*>{getIntTy(Mod)}, false));
  llvm::Function* mackefork = llvm::cast<llvm::Function>(ck);
  mackefork->setCallingConv(llvm::CallingConv::C);

  // Create main block that contains the switch statement
  llvm::BasicBlock* mainblock =
      llvm::BasicBlock::Create(Mod->getContext(), "", mackefork);
  llvm::IRBuilder<> mainbuilder(mainblock);

  // Create all blocks inside the case statements
  const int sizes[] = {1, 2, 4, 16, 256};
  llvm::BasicBlock* cases[sizeof(sizes) / sizeof(int)];

  for (int i = 0; i < sizeof(sizes) / sizeof(int); i++) {
    cases[i] = llvm::BasicBlock::Create(Mod->getContext(), "", mackefork);
    llvm::IRBuilder<> casebuilder(cases[i]);
    casebuilder.CreateRet(getInt(sizes[i], Mod, &casebuilder));
  }

  // Add all blocks to the switch case statement
  llvm::SwitchInst* theswitch = mainbuilder.CreateSwitch(
      mackefork->arg_begin(), cases[0], sizeof(sizes) / sizeof(int));

  for (int i = 0; i < sizeof(sizes) / sizeof(int); i++) {
    theswitch->addCase(getInt(sizes[i], Mod, &mainbuilder), cases[i]);
  }

  return mackefork;
}
