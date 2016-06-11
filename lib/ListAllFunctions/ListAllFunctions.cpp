#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct ListAllFunctions : public FunctionPass {
  static char ID;
  ListAllFunctions() : FunctionPass(ID) {
  }

  bool runOnFunction(Function &F) override {
    errs() << "Hello: ";
    errs().write_escaped(F.getName()) << '\n';
    return false;
  }
};

}  // namespace

char ListAllFunctions::ID = 0;
static RegisterPass<ListAllFunctions> X("listallfunctions",
                                        "List all functions", false, false);
