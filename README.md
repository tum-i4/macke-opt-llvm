# Passes for LLVM operations from MACKE


## How to build the project?

```
mkdir build
cd build
cmake -DLLVM_DIR="~/build/llvm" ..
make
```


## Example usage
```
opt -load lib/libMackeOpt.so -listallfuncs examples/divisible.bc -o /dev/null
```


## Further informations and readings

* [LLVM-OPT](http://llvm.org/docs/CommandGuide/opt.html)
* [Tutorial on writing a pass](http://llvm.org/docs/WritingAnLLVMPass.html)
* [Some details for external packages in cmake](http://wiki.icub.org/wiki/CMake_and_FIND_PACKAGE)
