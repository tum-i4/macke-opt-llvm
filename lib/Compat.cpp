
#include "Compat.h"

/**
 * Returns the name of an argument
 */
std::string getArgumentName(const llvm::Module* M, const llvm::Argument* argument)
{
  std::string ret = "argno" + std::to_string(argument->getArgNo());
  return ret;
}
