#include <map>
#include <set>
#include <string>
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"


namespace {

struct ExtractLinesOfCode : public llvm::ModulePass {
  static char ID;
  ExtractLinesOfCode() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  // FunctionName => Filename => LineNumber
  std::map<std::string, std::map<std::string, std::set<uint> > > result;

  bool doInitialization(llvm::Module &M) {
    result = {};
    return false;  // nothing was modified in the module
  }

  bool runOnModule(llvm::Module &M) override {
    for (auto &function : M) {
      if (!function.empty()) {
        std::map<std::string, std::set<uint> > resultnode{};

        for (auto &basicblock : function) {
          for (auto &instruction : basicblock) {
            if (llvm::MDNode *N = instruction.getMetadata("dbg")) {
              llvm::DILocation Loc(N);

              std::string dir = Loc.getDirectory().str();
              std::string file = Loc.getFilename().str();
              std::string identifier = dir;
              if (dir.back() != '/') {
                identifier += '/';
              }
              identifier += file;

              auto search = resultnode.find(identifier);
              if (search != resultnode.end()) {
                search->second.insert(Loc.getLineNumber());
              } else {
                resultnode[identifier] = std::set<uint>{Loc.getLineNumber()};
              }
            }
          }
        }
        // Add the result node to the final result
        result[function.getName()] = resultnode;
      }
    }
  }

  bool doFinalization(llvm::Module &M) {
    llvm::outs() << '{';

    bool ritfirst = true;
    for (auto &rit : result) {
      if (!ritfirst) {
        llvm::outs() << ',';
      } else {
        ritfirst = false;
      }
      llvm::outs() << '"' << rit.first << '"' << ':';

      bool fitfirst = true;
      llvm::outs() << '{';
      for (auto &fit : rit.second) {
        if (!fitfirst) {
          llvm::outs() << ',';
        } else {
          fitfirst = false;
        }
        llvm::outs() << '"' << fit.first << '"' << ':' << '[';

        bool litfirst = true;
        for (auto &lit : fit.second) {
          if (!litfirst) {
            llvm::outs() << ',';
          } else {
            litfirst = false;
          }
          llvm::outs() << lit;
        }
        llvm::outs() << ']';
      }
      llvm::outs() << '}';
    }
    llvm::outs() << '}' << '\n';

    return false;  // nothing was modified in the module
  }
};
}  // namespace


// Finally register the new pass
char ExtractLinesOfCode::ID = 0;
static llvm::RegisterPass<ExtractLinesOfCode> X(
    "extractlinesofcode", "Extract all lines of code represented as bitcode",
    true,  // walks CFG without modifying it?
    true   // is only analysis?
    );
