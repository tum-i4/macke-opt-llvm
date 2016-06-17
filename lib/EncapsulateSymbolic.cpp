#include <string>
#include <vector>
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

static llvm::cl::opt<std::string> EncapsulatedFunction(
    "encapsulatedfunction",
    llvm::cl::desc("Name of the function to be encapsulated"));


struct EncapsulateSymbolic : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  EncapsulateSymbolic() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module& M) {
    if (EncapsulatedFunction.empty()) {
      llvm::errs() << "Error: -encapsulatedfunction paramter is needed!"
                   << '\n';
      return false;
    }

    // Look for the function to be encapsulated
    llvm::Function* toencapsulate = M.getFunction(EncapsulatedFunction);

    // Check if the function given by the user really exists
    if (toencapsulate == nullptr) {
      llvm::errs() << "Error: " << EncapsulatedFunction
                   << " is no function inside the module. " << '\n'
                   << "Encapsulation is not possible!" << '\n';
      return false;
    }

    // Look for an old main method
    llvm::Function* oldmain = M.getFunction("main");

    if (oldmain != nullptr) {
      // If an old main exist, rename it ...
      oldmain->setName("__main_old");
    }

    // Create the encapsulation
    llvm::IRBuilder<> modulebuilder(M.getContext());

    // The capsule is "void main()"
    llvm::Constant* c = M.getOrInsertFunction(
        "main", llvm::FunctionType::get(modulebuilder.getVoidTy(), false));
    llvm::Function* newmain = llvm::cast<llvm::Function>(c);
    newmain->setCallingConv(llvm::CallingConv::C);

    // Create a new basic block inside the new main
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(M.getContext(), "", newmain);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value*> newargs = {};

    // For all arguments of the function we want to encapsulate
    for (auto& argument : toencapsulate->getArgumentList()) {
      // Allocate new storage
      llvm::AllocaInst* alloc = builder.CreateAlloca(
          argument.getType(), 0, argument.getValueName()->first());
      // And store the a load to the variable
      newargs.push_back(builder.CreateLoad(alloc));
    }

    // Call our encapsulated function with the newly generated arguments
    builder.CreateCall(toencapsulate, llvm::ArrayRef<llvm::Value*>(newargs));

    // The new main has void return type
    builder.CreateRetVoid();

    return true;  // This module was modified
  }
};

}  // namespace

// Finally register the new pass
char EncapsulateSymbolic::ID = 0;
static llvm::RegisterPass<EncapsulateSymbolic> X(
    "encapsulatesymbolic",
    "Add a new main that calls a function with symbolic arguments",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
