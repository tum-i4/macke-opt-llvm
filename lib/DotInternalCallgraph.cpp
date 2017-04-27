// based on llvm/lib/Analysis/IPA/CallPrinter.cpp

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/DOTGraphTraitsPass.h"

using namespace llvm;

namespace llvm {

template <>
struct DOTGraphTraits<CallGraph *> : public DefaultDOTGraphTraits {
  DOTGraphTraits(bool isSimple = false) : DefaultDOTGraphTraits(isSimple) {
  }

  static std::string getGraphProperties(CallGraph *Graph) {
    // Do not print duplicate edges
    return "concentrate=true;";
  }

  static bool isNodeHidden(CallGraphNode *Node) {
    Function *Func = Node->getFunction();
    // Nodes without a function and external functions should be hidden
    return !Func || Func->empty();
  }

  std::string getNodeLabel(CallGraphNode *Node, CallGraph *Graph) {
    if (Function *Func = Node->getFunction())
      return Func->getName();

    return "external node";
  }
};

}  // end llvm namespace

namespace {

struct InternalCallGraphPrinter
    : public DOTGraphTraitsModulePrinter<CallGraph, true> {
  static char ID;

  InternalCallGraphPrinter()
      : DOTGraphTraitsModulePrinter<CallGraph, true>("callgraph", ID) {
    initializeCallGraphPrinterPass(*PassRegistry::getPassRegistry());
  }
};

}  // end anonymous namespace

char InternalCallGraphPrinter::ID = 0;
static llvm::RegisterPass<InternalCallGraphPrinter> X(
    "dot-internal-callgraph",
    "Print call graph for internal functions to 'dot' file",
    true,  // walks CFG without modifying it
    true   // is only analysis
    );
