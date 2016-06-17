# Passes for LLVM operations from MACKE


## How to build the project?

```
make
```
and if you want to run some tests

```
make test
```

## Example usage

After running `make` (of course)

```
opt -load lib/libMackeOpt.so -listallfuncs examples/divisible.bc -disable-output

opt -load bin/libMackeOpt.so -changeentrypoint bin/greetings.bc -nef french -o bin/mod.bc

opt -load bin/libMackeOpt.so -encapsulatesymbolic bin/not42.bc -encapsulatedfunction not42 -o bin/not42-klee.bc
klee bin/not42-klee.bc
```

## Hints for developpers

Please run `make format` before your commits


## Further informations and readings

* [LLVM-OPT](http://llvm.org/docs/CommandGuide/opt.html)
* [Tutorial on writing a pass](http://llvm.org/docs/WritingAnLLVMPass.html)
* [Some details for external packages in cmake](http://wiki.icub.org/wiki/CMake_and_FIND_PACKAGE)
* [Small introduction to LLVM](http://adriansampson.net/blog/llvm.html)
* [Hello World with IR-Builder](https://www.ibm.com/developerworks/library/os-createcompilerllvm1/index.html)
