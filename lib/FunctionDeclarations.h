#ifndef LIB_FUNCTIONDECLARATIONS_H_
#define LIB_FUNCTIONDECLARATIONS_H_

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

llvm::Function* declare_klee_int(llvm::Module* Mod);
llvm::Function* declare_klee_make_symbolic(llvm::Module* Mod);
llvm::Function* declare_klee_range(llvm::Module* Mod);
llvm::Function* declare_malloc(llvm::Module* Mod);
llvm::Function* declare_free(llvm::Module* Mod);
llvm::Function* declare_memcmp(llvm::Module* Mod);
llvm::Function* declare_klee_silent_exit(llvm::Module* Mod);
llvm::Function* declare_klee_report_error(llvm::Module* Mod);
llvm::Function* declare_klee_get_obj_size(llvm::Module* Mod);

#endif  // LIB_FUNCTIONDECLARATIONS_H_
