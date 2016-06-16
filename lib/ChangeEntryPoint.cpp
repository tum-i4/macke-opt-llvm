#include <list>
#include <string>
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

bool hasValidMainType(llvm::Function* Fn) {
  // based on ExecutionEngine::runFunctionAsMain

  unsigned NumArgs = Fn->getFunctionType()->getNumParams();

  llvm::FunctionType* FTy = Fn->getFunctionType();
  llvm::Type* PPInt8Ty =
      llvm::Type::getInt8PtrTy(Fn->getContext())->getPointerTo();

  if (NumArgs > 3) {
    // Invalid number of arguments of main() supplied
    return false;
  }

  if (NumArgs >= 3 && FTy->getParamType(2) != PPInt8Ty) {
    // Invalid type for third argument of main() supplied
    return false;
  }

  if (NumArgs >= 2 && FTy->getParamType(1) != PPInt8Ty) {
    // Invalid type for second argument of main() supplied
    return false;
  }

  if (NumArgs >= 1 && !FTy->getParamType(0)->isIntegerTy(32)) {
    // Invalid type for first argument of main() supplied
    return false;
  }

  if (!FTy->getReturnType()->isIntegerTy() &&
      !FTy->getReturnType()->isVoidTy()) {
    // Invalid return type of main() supplied
    return false;
  }

  return true;
}


llvm::cl::opt<std::string> NewEntryFunction(
    "nef", llvm::cl::desc("Name of the function used as new entry point"),
    llvm::cl::value_desc("newentryfunction"), llvm::cl::Required);

struct ChangeEntryPoint : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  ChangeEntryPoint() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module& M) {
    // Look for an old main method
    llvm::Function* oldmain = M.getFunction("main");

    if (oldmain != nullptr) {
      // If an old main exist, rename it ...
      oldmain->setName("__main_old");
    }

    // Look for the new entry point function given by the user
    llvm::Function* newentryfunc = M.getFunction(NewEntryFunction);

    // Check if the function given by the user really exists
    if (newentryfunc == nullptr) {
      llvm::errs() << "Error: " << NewEntryFunction
                   << " is no function inside the module. " << '\n'
                   << "Entry point was not changed!" << '\n';
      return false;
    }

    // Check, if signature can be used as main function
    if (!hasValidMainType(newentryfunc)) {
      llvm::errs() << "Error: " << NewEntryFunction
                   << " do not have the signature for a main function." << '\n'
                   << "Entry point was not changed!" << '\n';
    }

    // Create wrapper function as a new entry function
    // The new main is has the same signature and attributes as new entry func
    llvm::Constant* c =
        M.getOrInsertFunction("main", newentryfunc->getFunctionType());
    llvm::Function* newmain = llvm::cast<llvm::Function>(c);
    newmain->setCallingConv(llvm::CallingConv::C);
    newmain->copyAttributesFrom(newentryfunc);

    // Create a new basic block inside the new main
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(M.getContext(), "", newmain);
    llvm::IRBuilder<> builder(block);

    // Add a call to the new entry point
    llvm::Value* ret = builder.CreateCall(newentryfunc);

    // Add return statement if new entry point has one
    if (newentryfunc->getReturnType() == builder.getVoidTy()) {
      builder.CreateRetVoid();
    } else {
      builder.CreateRet(ret);
    }

    llvm::outs() << "New entry point set to " << NewEntryFunction << '\n';

    return true;  // This module was modified
  }
};

}  // namespace

// Finally register the new pass
char ChangeEntryPoint::ID = 0;
static llvm::RegisterPass<ChangeEntryPoint> X(
    "changeentrypoint", "Change the entry point of the program",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
