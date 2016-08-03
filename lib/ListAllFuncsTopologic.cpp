#include <list>
#include "llvm/IR/Function.h"
// #include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Support/raw_ostream.h"

namespace {

void printAsJsonList(llvm::raw_ostream &out,
                     std::list<std::list<llvm::StringRef>> &doublelist) {
  // start json list
  out << '[';

  // little trick for the comma between elements
  bool first = true;

  for (auto &nestedlist : doublelist) {
    if (!first) {
      // Print the comma for the previous element
      out << ',';
    }

    // some indention for the list elements
    out << '\n' << "  ";

    // then write the escaped function name
    if (nestedlist.size() == 1) {
      // Do not nest a single entry inside a list
      out << '"';
      out.write_escaped(nestedlist.front());
      out << '"';
    } else {
      // Nest multiple entries inside a second list

      // Print head of the list and the first element
      out << '[' << '"';
      out.write_escaped(nestedlist.front());

      // Concat the rest of the list to the output
      nestedlist.pop_front();
      for (auto &elem : nestedlist) {
        out << '"' << ", " << '"';
        out.write_escaped(elem);
      }

      // And terminate the nested list
      out << '"' << ']';
    }

    // Terminate this element with a comma
    first = false;
  }

  // end the json list
  out << '\n' << ']' << '\n';
}


struct ListAllFuncsTopologic : public llvm::CallGraphSCCPass {
  static char ID;  // uninitialized ID is needed for pass registration
  std::list<std::list<llvm::StringRef>> funclist;

  ListAllFuncsTopologic() : CallGraphSCCPass(ID) {
    /* empty constructor, just call the parent's one */
  }


  bool doInitialization(llvm::Module &M) {
    // Strange, but avoids warnings about hidden functions
  }

  bool doFinalization(llvm::Module &M) {
    // Strange, but avoids warnings about hidden functions
  }

  bool doInitialization(llvm::CallGraph &CG) override {
    funclist = {};
    return false;  // nothing was modified in the module
  }

  bool runOnSCC(llvm::CallGraphSCC &SCC) override {
    std::list<llvm::StringRef> containedFunctions = {};

    // Iterate over all call graph nodes in the current component
    for (auto &cgn : SCC) {
      auto func = cgn->getFunction();

      // Only add real, non extern functions
      if (func != nullptr && func->size() != 0) {
        containedFunctions.push_back(func->getName());
      }
    }

    // Store all extracted functions
    if (!containedFunctions.empty()) {
      containedFunctions.sort();
      this->funclist.push_back(containedFunctions);
    }

    return false;  // nothing was modified in the function
  }

  bool doFinalization(llvm::CallGraph &CG) override {
    // function list as json goes to raw output stream
    printAsJsonList(llvm::outs(), funclist);

    return false;  // nothing was modified in the module
  }
};

}  // namespace

// Finally register the new pass
char ListAllFuncsTopologic::ID = 0;
static llvm::RegisterPass<ListAllFuncsTopologic> X(
    "listallfuncstopologic", "List all functions in topological order",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
