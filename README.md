# Passes for LLVM operations from MACKE


## How to build the project?

```
mkdir build
cd build
cmake ..
make
```


## Example usage
```
opt -load lib/libMackeOpt.so -listallfuncs examples/divisible.bc -o /dev/null
```


## Further informations and readings

* [LLVM-OPT](http://llvm.org/docs/CommandGuide/opt.html)
* [Tutorial on writing a pass](http://llvm.org/docs/WritingAnLLVMPass.html)
