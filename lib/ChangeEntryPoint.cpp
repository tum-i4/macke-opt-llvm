#include <list>
#include <string>
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

llvm::cl::opt<std::string> NewEntryFunction(
    "nef", llvm::cl::desc("Name of the function used as new entry point"),
    llvm::cl::value_desc("newentryfunction"), llvm::cl::Required);

struct ChangeEntryPoint : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  ChangeEntryPoint() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module &M) {
    // Generate the callgraph for the given Module
    llvm::outs() << "Function used as new entry point: " << NewEntryFunction
                 << '\n';

    // Check that functions exist

    llvm::Function *oldmain = M.getFunction("main");
    llvm::Function *newmain = M.getFunction(NewEntryFunction);

    // TODO check arguments etc. are matching

    newmain->takeName(oldmain);
    oldmain->setName("oldmain");

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
