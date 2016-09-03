#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Arch64or32bit.h"
#include "DirectoryHelper.h"
#include "FunctionDeclarations.h"
#include "MackeKTest.h"
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

static llvm::cl::list<std::string> PreviousKleeRunDirectory(
    "previouskleerundirectory",
    llvm::cl::desc("klee-out-XX directory of a previous run"));

static llvm::cl::list<std::string> ErrorFileToPrepend(
    "errorfiletoprepend",
    llvm::cl::desc("klee .err file, that should be prepended"));

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

    if (PreviousKleeRunDirectory.empty() && ErrorFileToPrepend.empty()) {
      llvm::errs() << "Error: -previouskleerundirectory paramter or "
                   << " -errorfiletoprepend parameter is needed!" << '\n';
      return false;
    }

    for (auto& pkrd : PreviousKleeRunDirectory) {
      if (!is_valid_directory(pkrd.c_str())) {
        llvm::errs() << "Error: " << pkrd << " is not a valid directory"
                     << '\n';
        return false;
      }
    }

    for (auto& eftp : ErrorFileToPrepend) {
      if (!hasEnding(eftp.c_str(), ".err") || !is_valid_file(eftp.c_str())) {
        llvm::errs() << "Error: " << eftp << " is not a valid .err-file"
                     << '\n';
        return false;
      }
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
    llvm::Function* kleegetobjsize = declare_klee_get_obj_size(&M);

    // Create the function to be prepended
    llvm::Function* prependedFunc = llvm::Function::Create(
        backgroundFunc->getFunctionType(), backgroundFunc->getLinkage(),
        "__macke_error_" + backgroundFunc->getName(), &M);

    // Give the correct name to all the arguments
    auto oldarg = backgroundFunc->arg_begin();
    std::unordered_map<std::string, llvm::Value*> variablemap;
    for (auto& newarg : prependedFunc->getArgumentList()) {
      auto oldname = llvm::cast<llvm::Value>(oldarg)->getName();
      newarg.setName(oldname);
      variablemap[oldname] = &newarg;
      ++oldarg;
    }

    // Mark the prepended function as not inline-able
    prependedFunc->addFnAttr(llvm::Attribute::NoInline);
    prependedFunc->addFnAttr(llvm::Attribute::OptimizeNone);
    // And also mark the background function - its needed for targeted search
    backgroundFunc->addFnAttr(llvm::Attribute::NoInline);
    backgroundFunc->addFnAttr(llvm::Attribute::OptimizeNone);

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

    // Read all required test data
    std::list<std::pair<std::string, std::string>> errlist = {};

    // From complete directories
    for (auto& pkrd : PreviousKleeRunDirectory) {
      auto newtests = errors_and_ktests_from_dir(pkrd);
      for (auto& nt : newtests) {
        errlist.push_back(nt);
      }
    }

    // From explicitly named error files
    for (auto& eftp : ErrorFileToPrepend) {
      errlist.push_back(std::make_pair(eftp, corresponding_ktest(eftp)));
    }

    llvm::outs() << "Hello world!" << '\n';
    llvm::outs() << errlist.size() << '\n';
    for (auto& ps : errlist) {
      llvm::outs() << ps.first << " - " << ps.second << '\n';
    }

    // Create the switch statement to fork for each prepended error
    llvm::SwitchInst* theswitch =
        builder.CreateSwitch(symswitchvar, defaultblock, errlist.size() + 1);

    // One branch statement for each ktest file
    uint counter = 1;
    for (auto& errfile : errlist) {
      llvm::BasicBlock* caseblock =
          llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
      llvm::IRBuilder<> casebuilder(caseblock);

      // Block for comparing the error size
      llvm::BasicBlock* checkcontentblock =
          llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
      llvm::IRBuilder<> checkcontentbuilder(checkcontentblock);

      // The value for checking, if the sizes does match
      llvm::Value* correctsize = casebuilder.getTrue();
      // The value for checking, if the content does match
      llvm::Value* correctcontent = casebuilder.getTrue();

      // Load the date from the corresponding ktest file
      MackeKTest ktest = MackeKTest(errfile.second.c_str());

      // For each variable defined in the ktest objecct
      for (auto& kobj : ktest.objects) {
        // Ignore all variables starting with MACKE and
        //  model_version, A-data, A-data-stat,
        //  stdin, stdin-stat from posix environment
        if (kobj.name.substr(0, 6) != "macke_" &&
            kobj.name != "model_version" && kobj.name != "A-data" &&
            kobj.name != "A-data-stat" && kobj.name != "stdin" &&
            kobj.name != "stdin-stat" && kobj.name != "macke_noname") {
          // Search for a matching variable in the function
          auto search = variablemap.find(kobj.name);
          if (search != variablemap.end()) {
            llvm::Value* fvalue = search->second;
            llvm::Value* fvalueptr;

            if (fvalue->getType()->isPointerTy()) {
              // If it is already a pointer, just keep working with it
              fvalueptr = fvalue;
            } else {
              // If its not a pointer, we add one level of indirection
              fvalueptr = casebuilder.CreateAlloca(fvalue->getType());
              casebuilder.CreateStore(fvalue, fvalueptr);
            }

            // Cast the pointer to void*
            fvalueptr = casebuilder.CreateBitCast(
                fvalueptr, builder.getInt8Ty()->getPointerTo());

            // Compare the object size
            correctsize = casebuilder.CreateAnd(
                correctsize, casebuilder.CreateICmpEQ(
                                 casebuilder.CreateCall(
                                     kleegetobjsize,
                                     llvm::ArrayRef<llvm::Value*>(fvalueptr)),
                                 getInt(kobj.value.size(), &M, &casebuilder)));

            // Compare the object content
            for (int i = 0; i < kobj.value.size(); i++) {
              // Get element pointer with the right element offset,
              // load the memory, and  Compare it
              correctcontent = checkcontentbuilder.CreateAnd(
                  correctcontent,
                  checkcontentbuilder.CreateICmpEQ(
                      checkcontentbuilder.CreateLoad(
                          checkcontentbuilder.CreateConstGEP1_64(fvalueptr, i)),
                      checkcontentbuilder.getInt8(kobj.value[i])));
            }

          } else {
            llvm::errs() << "ERROR: KTest variable " << kobj.name
                         << " not found in function" << '\n';
            abort();
          }
        }
      }

      // The error reporting if block
      llvm::BasicBlock* throwerrblock =
          llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
      llvm::IRBuilder<> throwerrbuilder(throwerrblock);

      throwerrbuilder.CreateCall(
          kleereporterror,
          llvm::ArrayRef<llvm::Value*>(std::vector<llvm::Value*>{
              throwerrbuilder.CreateGlobalStringPtr("MACKE"),
              throwerrbuilder.getInt32(0),
              throwerrbuilder.CreateGlobalStringPtr("ERROR FROM " +
                                                    errfile.first),
              throwerrbuilder.CreateGlobalStringPtr("macke.err"),
          }));
      throwerrbuilder.CreateUnreachable();

      // The no error else block
      llvm::BasicBlock* noterrblock =
          llvm::BasicBlock::Create(M.getContext(), "", prependedFunc);
      llvm::IRBuilder<> noterrbuilder(noterrblock);

      noterrbuilder.CreateCall(kleesilentexit, llvm::ArrayRef<llvm::Value*>(
                                                   noterrbuilder.getInt32(0)));
      noterrbuilder.CreateUnreachable();

      // build the branches for size and content check
      casebuilder.CreateCondBr(correctsize, checkcontentblock, noterrblock);
      checkcontentbuilder.CreateCondBr(correctcontent, throwerrblock,
                                       noterrblock);

      // Finally add the case block
      theswitch->addCase(casebuilder.getInt32(counter), caseblock);
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
