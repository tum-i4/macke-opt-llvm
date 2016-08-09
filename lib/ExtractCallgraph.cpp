#include <list>
#include <map>
#include <string>
#include <utility>
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

namespace {

static const std::string NULLFUNC = "null function";

// Small struct to store content for later json output
struct Node {
  std::string name = "";
  uint uses = 0;
  std::list<std::string> calls = {};
  std::list<std::string> calledby = {};
  bool hassingleptrarg = false;
  bool hasdoubleptrarg = false;
  bool isexternal = true;
};

std::string json(const Node &node) {
  // Initialize output
  std::string result = "";

  // name is the key of the set element
  result += '"' + node.name + '"' + ':';

  // start the internal object
  result += '{';

  // append number of usages
  result += "\"#uses\":" + std::to_string(node.uses) + ',';

  // append information about single pointers
  result += "\"hassingleptrarg\":";
  result += (node.hassingleptrarg) ? "true" : "false";
  result += ',';


  // append information about double pointers
  result += "\"hasdoubleptrarg\":";
  result += (node.hasdoubleptrarg) ? "true" : "false";
  result += ',';

  // append information about external functions
  result += "\"isexternal\":";
  result += (node.isexternal) ? "true" : "false";
  result += ',';

  // start call list
  result += "\"calls\":[";

  // small trick for the separating comma
  bool first = true;

  for (auto &call : node.calls) {
    if (!first) {
      result += ',';
    }

    result += '"' + call + '"';
    first = false;
  }

  // close the call list
  result += "],";

  // start called by list
  result += "\"calledby\":[";

  // small trick for the separating comma
  first = true;

  for (auto &called : node.calledby) {
    if (!first) {
      result += ',';
    }

    result += '"' + called + '"';
    first = false;
  }

  // close the calledby list
  result += ']';

  // close the internal object
  result += '}';

  return result;
}

std::string json(const std::list<Node> &nodes) {
  // Initialize output
  std::string result = "";

  // start the map
  result += '{';

  // small trick for the separating comma
  bool first = true;

  // Sort the nodes alphabetically
  auto sortedNodes = nodes;
  sortedNodes.sort([](Node lhs, Node rhs) {
    // null functions come first
    if (lhs.name == NULLFUNC) {
      return true;
    } else if (rhs.name == NULLFUNC) {
      return false;
    }

    return lhs.name < rhs.name;
  });

  // print each node
  for (auto &node : sortedNodes) {
    if (!first) {
      result += ',';
    }

    result += json(node);
    first = false;
  }

  // close the map
  result += '}';

  return result;
}

struct ExtractCallgraph : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  ExtractCallgraph() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module &M) {
    // Generate the callgraph for the given Module
    llvm::CallGraph cg{};
    cg.runOnModule(M);

    // Print default representation for callgraph for debugging purpose
    // llvm::outs() << "\n\n\n";
    // cg.print(llvm::outs(), &M);
    // llvm::outs() << "\n\n\n";

    std::list<Node> result = {};
    std::multimap<std::string, std::string> calledby = {};

    for (auto &cgp : cg) {
      // type is std::pair<const llvm::Function*, llvm::CallGraphNode*>
      llvm::CallGraphNode *cgn = cgp.second;

      // Initialize empty node
      Node thisnode = {};

      // Read the name of the function
      if (llvm::Function *F = cgn->getFunction()) {
        thisnode.name = F->getName();

        // Extract information about ptr arguments
        for (auto &argument : F->getArgumentList()) {
          if (argument.getType()->isPointerTy()) {
            if (argument.getType()->getPointerElementType()->isPointerTy()) {
              thisnode.hasdoubleptrarg = true;
            } else {
              thisnode.hassingleptrarg = true;
            }
          }
        }
        thisnode.isexternal = F->empty();

      } else {
        thisnode.name = NULLFUNC;
      }

      // Extract number of uses
      thisnode.uses = cgn->getNumReferences();

      // Iterate over all functions, that are called in the node
      for (auto I = cgn->begin(), E = cgn->end(); I != E; ++I) {
        // type is struct std::pair<llvm::WeakVH, llvm::CallGraphNode*>

        // Extract the name of the external node
        if (llvm::Function *FI = I->second->getFunction()) {
          thisnode.calls.push_back(I->second->getFunction()->getName());
          if (thisnode.name != NULLFUNC) {
            calledby.insert(std::make_pair(
              I->second->getFunction()->getName(), thisnode.name));
          }
        }
      }
      thisnode.calls.sort();
      thisnode.calls.unique();

      // Store this node in the result
      result.push_back(thisnode);
    }

    // Add called by information to all function nodes
    for (auto &r : result) {
      auto range = calledby.equal_range(r.name);
      for (auto& c = range.first; c != range.second; ++c) {
        r.calledby.push_back(c->second);
      }
      // Sort the results and remove duplicates
      r.calledby.sort();
      r.calledby.unique();
    }

    // Print json-representation of the callgraph
    llvm::outs() << json(result) << '\n';

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
