# These are the only two external values to be set
LLVM_SRC_PATH ?= $$HOME/build/llvm
KLEE_BIN_PATH ?= $$HOME/build/klee/Release+Asserts/bin
KLEE_INLCUDES ?= $$HOME/build/klee/include/

# Setting some variables and commands for compilaten
LLVM_BUILD_PATH = $(LLVM_SRC_PATH)/Release
LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/bin
LLVM_INCLUDES = -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include

CXX = g++
CXXFLAGS_LLVM = -fno-rtti -O0 $(LLVM_INCLUDES)

LLVM_CONFIG_COMMAND = \
		`$(LLVM_BIN_PATH)/llvm-config --cxxflags --libs` \
		`$(LLVM_BIN_PATH)/llvm-config --ldflags`



all: bin/libMackeOpt.so \
	bin/divisible.bc bin/greetings.bc bin/not42.bc bin/assertions.bc \
	bin/klee_objsize.bc bin/klee_stacktrace.bc

.PHONY: test
test: all
	@ LLVMBIN=$(LLVM_BIN_PATH) KLEEBIN=$(KLEE_BIN_PATH) python -m unittest

bin/libMackeOpt.so: \
		bin/Arch64or32bit.o \
		bin/ChangeEntryPoint.o \
		bin/EncapsulateSymbolic.o \
		bin/ExtractCallgraph.o \
		bin/FunctionDeclarations.o \
		bin/ListAllFunctions.o \
		bin/PrependError.o
	$(CXX) -std=c++11 $(CXXFLAGS_LLVM) -shared $(LLVM_CONFIG_COMMAND) $^ -o $@


bin/%.o: lib/%.cpp
	$(CXX) -c -fPIC -std=c++11 $(CXXFLAGS_LLVM) $(LLVM_CONFIG_COMMAND) $^ -o $@


bin/%.bc: examples/%.c
	$(LLVM_BIN_PATH)/clang -c -emit-llvm -O0 -g $^ -o $@

bin/klee_%.bc: examples/klee_%.c
	$(LLVM_BIN_PATH)/clang -c -emit-llvm -I$(KLEE_INLCUDES) -O0 -g $^ -o $@

.PHONY: clean
clean:
	@ rm -rf bin
	@ git checkout bin/.gitignore

.PHONY: format
format:
	clang-format -i $(wildcard **/*.cpp) $(wildcard **/*.c) $(wildcard **/*.h)
