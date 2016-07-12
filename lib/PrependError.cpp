#include <string>
#include <vector>
#include "Arch64or32bit.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

static llvm::cl::opt<std::string> PrependToFunction(
    "prependtofunction",
    llvm::cl::desc("Name of the function that is prepended with the errors"));


struct PrependError : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  PrependError() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module& M) {
    if (PrependToFunction.empty()) {
      llvm::errs() << "Error: -prependtofunction paramter is needed!" << '\n';
      return false;
    }

    // Look for the function to be encapsulated
    llvm::Function* backgroundFunc = M.getFunction(PrependToFunction);

    // Check if the function given by the user really exists
    if (backgroundFunc == nullptr) {
      llvm::errs() << "Error: " << PrependToFunction
                   << " is no function inside the module. " << '\n'
                   << "Prepend is not possible!" << '\n';
      return false;
    }

    // Get builder for the whole module
    llvm::IRBuilder<> modulebuilder(M.getContext());

    // Create the function to be prepended
    llvm::Function* prependedFunc = llvm::Function::Create(
        backgroundFunc->getFunctionType(), backgroundFunc->getLinkage(),
        "__macke_error_" + backgroundFunc->getName(), &M);

    // Create a basic block in the prepended function
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
    llvm::IRBuilder<> builder(block);

    // Replace all calls to the original function with the prepended function
    backgroundFunc->replaceAllUsesWith(prependedFunc);

    // Build argument list for the prepended function
    std::vector<llvm::Value*> newargs = {};
    for (auto& argument : backgroundFunc->getArgumentList()) {
      newargs.push_back(&argument);
    }

    // Return the result of a original call to the function
    llvm::Instruction* origcall = builder.CreateCall(
        backgroundFunc, llvm::ArrayRef<llvm::Value*>(newargs));

    if (backgroundFunc->getFunctionType()->getReturnType()->isVoidTy()) {
      builder.CreateRetVoid();
    } else {
      builder.CreateRet(origcall);
    }

    return true;
  }
};

}  // namespace

// Finally register the new pass
char PrependError::ID = 0;
static llvm::RegisterPass<PrependError> X(
    "preprenderror",
    "Add a new main that calls a function with symbolic arguments",
    false,  // walks CFG without modifying it?
    false   // is only analysis?
    );
