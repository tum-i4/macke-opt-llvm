#include <iterator>
#include <string>
#include <vector>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "Arch64or32bit.h"
#include "FunctionDeclarations.h"
#include "FunctionDefinitions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"

namespace {

using namespace llvm;

cl::opt<std::string> functionName(
            "functionname",
            cl::desc("Name of the function for gathering internal attributes"));

struct FunctionInternalAttributes : public FunctionPass {
    static char ID;

    FunctionInternalAttributes(): FunctionPass(ID) {
        // Empty constructor
    }

    bool runOnFunction(Function& func) override {
        if (func.getName() != functionName) {
            return false;
        }

        unsigned int instCount = 0;
        unsigned int blockCount = 0;
        unsigned int pointerTypeCount = 0;

        for (BasicBlock& bb : func) {
            blockCount += 1;
            instCount += std::distance(bb.begin(), bb.end());
        }

        for (auto& argument : func.getArgumentList()) {
            if (argument.getType()->isPointerTy()) {
                pointerTypeCount += 1;
            }
        }

        outs() << instCount << "\n" << blockCount << "\n" << pointerTypeCount << "\n";

        return false;
    }
};
} // namespace

// register the new pass
char FunctionInternalAttributes::ID = 0;
static llvm::RegisterPass<FunctionInternalAttributes> X(
    "functioninternalattributes",
    "Gather some static features from function for analyzing it for severity assessment",
    true,  // walks CFG without modifying it?
    true   // is only analysis?
);
