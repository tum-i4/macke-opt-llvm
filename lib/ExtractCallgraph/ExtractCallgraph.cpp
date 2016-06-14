#include <list>
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/ADT/SmallVector.h"

namespace {

struct ExtractCallgraph : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration
  std::unique_ptr<llvm::CallGraph> cg;

  ExtractCallgraph() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module &M) {
    // Generate the callgraph for the given Module
    cg.reset(new llvm::CallGraph(M));

    // Print default representation for callgraph for debugging purpose
    llvm::outs() << "\n\n\n";
    cg->print(llvm::outs());
    llvm::outs() << "\n\n\n";

    //////////////////////////////

    llvm::SmallVector<llvm::CallGraphNode *, 16> Nodes;
    for (auto I = cg->begin(), E = cg->end(); I != E; ++I)
      Nodes.push_back(I->second.get());

    std::sort(Nodes.begin(), Nodes.end(),
      [](llvm::CallGraphNode *LHS, llvm::CallGraphNode *RHS) {
          if (llvm::Function *LF = LHS->getFunction())
            if (llvm::Function *RF = RHS->getFunction())
              return LF->getName() < RF->getName();

        return RHS->getFunction() != nullptr;
      });

      for (llvm::CallGraphNode *CN : Nodes) {
        CN->print(llvm::outs());
        CN->getNumReferences();
        llvm::outs() << CN->getNumReferences() << "\n";
        CN->print(llvm::outs());
      }


    ///////////////////////////////

    return false;  // nothing was modified in the module
  }
};

}  // namespace

// Finally register the new pass
char ExtractCallgraph::ID = 0;
static llvm::RegisterPass<ExtractCallgraph> X(
    "extractcallgraph", "Extract the Callgraph",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
