#include <string>
#include <vector>
#include "Arch64or32bit.h"
#include "FunctionDeclarations.h"
#include "FunctionDefinitions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace {

static llvm::cl::opt<std::string> EncapsulatedFunction(
    "encapsulatedfunction",
    llvm::cl::desc("Name of the function to be encapsulated"));


struct EncapsulateSymbolic : public llvm::ModulePass {
  static char ID;  // uninitialized ID is needed for pass registration

  EncapsulateSymbolic() : llvm::ModulePass(ID) {
    /* empty constructor, just call the parent's one */
  }

  bool runOnModule(llvm::Module& M) {
    if (EncapsulatedFunction.empty()) {
      llvm::errs() << "Error: -encapsulatedfunction paramter is needed!"
                   << '\n';
      return false;
    }

    // Look for the function to be encapsulated
    llvm::Function* toencapsulate = M.getFunction(EncapsulatedFunction);

    // Check if the function given by the user really exists
    if (toencapsulate == nullptr) {
      llvm::errs() << "Error: " << EncapsulatedFunction
                   << " is no function inside the module. " << '\n'
                   << "Encapsulation is not possible!" << '\n';
      return false;
    }

    // Get builder for the whole module
    llvm::IRBuilder<> modulebuilder(M.getContext());
    // Datalayout for size calculation
    llvm::DataLayout datalayout(&M);

    // Register relevant functions
    llvm::Function* kleemakesym = declare_klee_make_symbolic(&M);
    llvm::Function* mymalloc = declare_malloc(&M);
    llvm::Function* myfree = declare_free(&M);
    llvm::Function* mymemcmp = declare_memcmp(&M);
    llvm::Function* mackeforksizes = define_macke_fork_several_sizes(&M);
    llvm::Function* kleerange = declare_klee_range(&M);
    llvm::Function* kleesilentexit = declare_klee_silent_exit(&M);
    llvm::Function* kleegetobjsize = declare_klee_get_obj_size(&M);

    // Create the encapsulation

    // The capsule is "void main()"
    llvm::Constant* cm = M.getOrInsertFunction(
        "macke_" + EncapsulatedFunction + "_main",
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(M.getContext()),
            llvm::ArrayRef<llvm::Type*>{std::vector<llvm::Type*>{
                llvm::Type::getInt32Ty(M.getContext()),
                llvm::Type::getInt8PtrTy(M.getContext())->getPointerTo()}},
            false));
    llvm::Function* newmain = llvm::cast<llvm::Function>(cm);
    newmain->setCallingConv(llvm::CallingConv::C);

    // Mark the new main function as not inline-able
    newmain->addFnAttr(llvm::Attribute::NoInline);
    newmain->addFnAttr(llvm::Attribute::OptimizeNone);

    // Create a new basic block inside the new main
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(M.getContext(), "", newmain);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value*> newargs = {};
    std::vector<llvm::Instruction*> mallocs = {};

    // For all arguments of the function we want to encapsulate
    for (auto& argument : toencapsulate->getArgumentList()) {
      llvm::StringRef argname = (argument.hasName())
                                    ? argument.getValueName()->first()
                                    : "macke_noname";

      if (argument.getType()->isPointerTy()) {
        // Pointers are handled differently
        // We allocate different sizes of memory and make it symbolic directly

        // int rangecall = klee_range(0, 5, "n");
        llvm::Instruction* rangecall = builder.CreateCall(
            kleerange, llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
                           builder.getInt32(1), builder.getInt32(1025),
                           builder.CreateGlobalStringPtr("macke_sizeof_" +
                                                         argname.str())}});

        // int thisrange = macke_fork_several_sizes(rangecall)
        llvm::Instruction* thisrange =
            builder.CreateCall(mackeforksizes, rangecall);

        // Calculate the size, that should be allocated
        llvm::Value* memsize = builder.CreateMul(
            thisrange, getInt(datalayout.getTypeAllocSize(
                                  argument.getType()->getPointerElementType()),
                              &M, &builder));

        // Allocate new storage
        llvm::Instruction* malloc = builder.CreateCall(mymalloc, memsize);

        // Register the malloc for later call to free
        mallocs.push_back(malloc);

        // Add a call to make_klee_symbolic for the new variable
        builder.CreateCall(
            kleemakesym,
            llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
                malloc, memsize, builder.CreateGlobalStringPtr(argname)}});

        newargs.push_back(builder.CreateBitCast(malloc, argument.getType()));

      } else {
        // Allocate new storage
        llvm::AllocaInst* alloc =
            builder.CreateAlloca(argument.getType(), 0, argname);

        // Add a call to make_klee_symbolic for the new variable
        builder.CreateCall(
            kleemakesym,
            llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
                builder.CreateBitCast(alloc,
                                      builder.getInt8Ty()->getPointerTo()),
                getInt(datalayout.getTypeAllocSize(argument.getType()), &M,
                       &builder),
                builder.CreateGlobalStringPtr(argname)}});

        // And store a load of the variable for latter call
        newargs.push_back(builder.CreateLoad(alloc));
      }
    }

    // Call our encapsulated function with the newly generated arguments
    llvm::Value* encret = builder.CreateCall(
        toencapsulate, llvm::ArrayRef<llvm::Value*>(newargs));

    // Create two blocks for result processing
    llvm::BasicBlock* blockSilent =
        llvm::BasicBlock::Create(M.getContext(), "", newmain);
    llvm::IRBuilder<> builderSilent(blockSilent);
    builderSilent.CreateCall(kleesilentexit, llvm::ArrayRef<llvm::Value*>(
                                                 builderSilent.getInt32(0)));
    builderSilent.CreateUnreachable();

    llvm::BasicBlock* blockReturn =
        llvm::BasicBlock::Create(M.getContext(), "", newmain);
    llvm::IRBuilder<> builderReturn(blockReturn);

    // Prepare some space for the result
    llvm::Type* encrettype = toencapsulate->getReturnType();
    if (encrettype->isVoidTy()) {
      builder.CreateBr(blockReturn);
    } else if (encrettype->isPointerTy()) {
      llvm::Value* rsize = builder.CreateCall(
          kleegetobjsize, llvm::ArrayRef<llvm::Value*>(builder.CreateBitCast(
                              encret, builder.getInt8Ty()->getPointerTo())));

      llvm::Instruction* check = builder.CreateCall(mymalloc, rsize);
      mallocs.push_back(check);

      builder.CreateCall(
          kleemakesym,
          llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
              builder.CreateBitCast(check, builder.getInt8Ty()->getPointerTo()),
              rsize, builder.CreateGlobalStringPtr("macke_result")}});

      llvm::Value* rescheck = builder.CreateICmpEQ(
          builder.CreateCall(
              mymemcmp, llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
                            builder.CreateBitCast(
                                encret, builder.getInt8Ty()->getPointerTo()),
                            check, rsize}}),
          builder.getInt32(0));

      builder.CreateCondBr(rescheck, blockReturn, blockSilent);
    } else {
      // Non Pointer values
      llvm::AllocaInst* allocR =
          builder.CreateAlloca(encrettype, 0, "macke_result");
      builder.CreateCall(
          kleemakesym,
          llvm::ArrayRef<llvm::Value*>{std::vector<llvm::Value*>{
              builder.CreateBitCast(allocR,
                                    builder.getInt8Ty()->getPointerTo()),
              getInt(datalayout.getTypeAllocSize(encrettype), &M, &builder),
              builder.CreateGlobalStringPtr("macke_result")}});

      // Assign the returned value to the result symbolic value
      llvm::Value* rescheck =
          builder.CreateICmpEQ(encret, builder.CreateLoad(allocR));
      builder.CreateCondBr(rescheck, blockReturn, blockSilent);
    }

    // Free all previous malloc
    for (auto& malloc : mallocs) {
      builderReturn.CreateCall(myfree, malloc);
    }
    // The new main returns always zero
    builderReturn.CreateRet(builder.getInt32(0));

    return true;  // This module was modified
  }
};

}  // namespace

// Finally register the new pass
char EncapsulateSymbolic::ID = 0;
static llvm::RegisterPass<EncapsulateSymbolic> X(
    "encapsulatesymbolic",
    "Add a new main that calls a function with symbolic arguments",
    false,  // walks CFG without modifying it?
    false   // is only analysis?
    );
