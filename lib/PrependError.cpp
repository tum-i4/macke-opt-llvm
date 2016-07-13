#include <list>
#include <string>
#include <vector>
#include "Arch64or32bit.h"
#include "DirectoryHelper.h"
#include "FunctionDeclarations.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

static llvm::cl::opt<std::string> PrependToFunction(
    "prependtofunction",
    llvm::cl::desc("Name of the function that is prepended with the errors"));

static llvm::cl::opt<std::string> PreviousKleeRunDirectory(
    "previouskleerundirectory",
    llvm::cl::desc("klee-out-XX directory of a previous run"));

struct PrependError : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  PrependError() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module& M) {
    // Check all the command line arguments
    if (PrependToFunction.empty()) {
      llvm::errs() << "Error: -prependtofunction paramter is needed!" << '\n';
      return false;
    }

    if (PreviousKleeRunDirectory.empty()) {
      llvm::errs() << "Error: -previouskleerundirectory paramter is needed!"
                   << '\n';
      return false;
    }

    if (!is_valid_directory(PreviousKleeRunDirectory.c_str())) {
      llvm::errs() << "Error: " << PreviousKleeRunDirectory
                   << " is not a valid directory" << '\n';
      return false;
    }

    // Look for the function to be encapsulated
    llvm::Function* backgroundFunc = M.getFunction(PrependToFunction);

    // Check if the function given by the user really exists
    if (backgroundFunc == nullptr) {
      llvm::errs() << "Error: " << PrependToFunction
                   << " is no function inside the module. " << '\n'
                   << "Prepend is not possible!" << '\n';
      return false;
    }

    // Get builder for the whole module
    llvm::IRBuilder<> modulebuilder(M.getContext());

    // Register relevant functions
    llvm::Function* kleeint = declare_klee_int(&M);
    llvm::Function* kleereporterror = declare_klee_report_error(&M);
    llvm::Function* kleesilentexit = declare_klee_silent_exit(&M);

    // Create the function to be prepended
    llvm::Function* prependedFunc = llvm::Function::Create(
        backgroundFunc->getFunctionType(), backgroundFunc->getLinkage(),
        "__macke_error_" + backgroundFunc->getName(), &M);

    // Give the correct name to all the arguments
    auto oldarg = backgroundFunc->arg_begin();
    for (auto& newarg : prependedFunc->getArgumentList()) {
      newarg.setName(llvm::cast<llvm::Value>(oldarg)->getName());
      ++oldarg;
    }

    // Create a basic block in the prepended function
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
    llvm::IRBuilder<> builder(block);

    // Build symbolic variable to fork later
    llvm::Instruction* symswitchvar = builder.CreateCall(
        kleeint, builder.CreateGlobalStringPtr(
                     "macke_sym_switch_" + backgroundFunc->getName().str()));

    // Create a basic block for the default case
    llvm::BasicBlock* defaultblock =
        llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
    llvm::IRBuilder<> defaultbuilder(defaultblock);

    // Replace all calls to the original function with the prepended function
    backgroundFunc->replaceAllUsesWith(prependedFunc);

    // Build argument list for the prepended function
    std::vector<llvm::Value*> newargs = {};
    for (auto& argument : prependedFunc->getArgumentList()) {
      newargs.push_back(&argument);
    }

    // Return the result of a original call to the function
    llvm::Instruction* origcall = defaultbuilder.CreateCall(
        backgroundFunc, llvm::ArrayRef<llvm::Value*>(newargs));

    if (backgroundFunc->getFunctionType()->getReturnType()->isVoidTy()) {
      defaultbuilder.CreateRetVoid();
    } else {
      defaultbuilder.CreateRet(origcall);
    }

    // Read all required test date
    std::list<std::string> ktestlist =
        error_ktests_from_dir(PreviousKleeRunDirectory);

    // Create the switch statement to fork for each prepended error
    llvm::SwitchInst* theswitch =
        builder.CreateSwitch(symswitchvar, defaultblock, ktestlist.size() + 1);

    // One branch statement for each ktest file
    uint counter = 1;
    for (auto& ktest : ktestlist) {
      llvm::BasicBlock* caseblock =
          llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
      llvm::IRBuilder<> casebuilder(caseblock);

      // The error reporting if block
      /*
      casebuilder.CreateCall(
          kleereporterror,
          llvm::ArrayRef<llvm::Value*>(std::vector<llvm::Value*>{
              casebuilder.CreateGlobalStringPtr("MACKE"),
              getInt(0, &M, &casebuilder),
              casebuilder.CreateGlobalStringPtr("message" +
                                                std::to_string(counter)),
              casebuilder.CreateGlobalStringPtr("macke.err"),
          }));
      casebuilder.CreateUnreachable();
      */

      // The no error else block
      casebuilder.CreateCall(kleesilentexit, llvm::ArrayRef<llvm::Value*>(
                                                 getInt(0, &M, &casebuilder)));
      casebuilder.CreateUnreachable();

      // Finally add the case block
      theswitch->addCase(getInt(counter, &M, &casebuilder), caseblock);
      counter++;
    }

    return true;
  }
};

}  // namespace

// Finally register the new pass
char PrependError::ID = 0;
static llvm::RegisterPass<PrependError> X(
    "preprenderror",
    "Add a new main that calls a function with symbolic arguments",
    false,  // walks CFG without modifying it?
    false   // is only analysis?
    );
