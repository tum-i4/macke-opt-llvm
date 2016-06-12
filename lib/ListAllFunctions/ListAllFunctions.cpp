#include <list>
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

namespace {

void printAsJsonList(llvm::raw_ostream &out,
                     std::list<llvm::StringRef> &elements) {
  // start json list
  out << '[';

  // little trick for the comma between elements
  bool first = true;

  for (auto &elem : elements) {
    if (!first) {
      // Print the comma for the previous element
      out << ',';
    }

    // some indention for the list elements
    out << '\n' << "  " << '"';

    // then write the escaped function name
    out.write_escaped(elem);

    // terminate the list entry
    out << '"';

    // Terminate this element with a comma
    first = false;
  }

  // end the json list
  out << '\n' << ']' << '\n';
}


struct ListAllFunctions : public llvm::FunctionPass {
  static char ID;  // uninitialized ID is needed for pass registration
  std::list<llvm::StringRef> funclist;

  ListAllFunctions() : FunctionPass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool doInitialization(llvm::Module &M) {
    funclist = {};
    return false;  // nothing was modified in the module
  }

  bool runOnFunction(llvm::Function &F) override {
    funclist.push_back(F.getName());
    return false;  // nothing was modified in the function
  }

  bool doFinalization(llvm::Module &M) {
    // sort the function list alphabetical
    funclist.sort();

    // function list as json goes to raw output stream
    printAsJsonList(llvm::outs(), funclist);

    return false;  // nothing was modified in the module
  }
};

}  // namespace

// Finally register the new pass
char ListAllFunctions::ID = 0;
static llvm::RegisterPass<ListAllFunctions> X(
    "listallfunctions", "List all functions",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
