
#ifndef LIB_COMPAT_H_
#define LIB_COMPAT_H_

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

/**
 * Returns the name of an argument
 */
std::string getArgumentName(const llvm::Module* M, const llvm::Argument* argument);

#endif  // LIB_COMPAT_H
