#include "FunctionDefinitions.h"
#include <vector>
#include "Arch64or32bit.h"
#include "Constants.h"


// Create a function equivalent to
// size_t foo(int n) {
//     switch(n) {
//         default:
//         case   1: return   1; break;
//         case   2: return   2; break;
//         ...
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

  // Check if the function body was alread defined
  if (mackefork->size() > 0) {
    return mackefork;
  }

  mackefork->setCallingConv(llvm::CallingConv::C);

  // Mark the new function as not inline-able
  mackefork->addFnAttr(llvm::Attribute::NoInline);
  mackefork->addFnAttr(llvm::Attribute::OptimizeNone);

  // Create main block that contains the switch statement
  llvm::BasicBlock* mainblock =
      llvm::BasicBlock::Create(Mod->getContext(), "", mackefork);
  llvm::IRBuilder<> mainbuilder(mainblock);

  // Create all blocks inside the case statements
  llvm::BasicBlock* cases[ptrforksizes.size()];

  for (int i = 0; i < ptrforksizes.size(); i++) {
    cases[i] = llvm::BasicBlock::Create(Mod->getContext(), "", mackefork);
    llvm::IRBuilder<> casebuilder(cases[i]);
    casebuilder.CreateRet(getInt(ptrforksizes[i], Mod, &casebuilder));
  }

  // Add all blocks to the switch case statement
  llvm::SwitchInst* theswitch = mainbuilder.CreateSwitch(
      mackefork->arg_begin(), cases[0], ptrforksizes.size());

  for (int i = 0; i < ptrforksizes.size(); i++) {
    theswitch->addCase(getInt(ptrforksizes[i], Mod, &mainbuilder), cases[i]);
  }

  return mackefork;
}
