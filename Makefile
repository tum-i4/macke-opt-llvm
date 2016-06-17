# This is the only external value to be set
LLVM_SRC_PATH ?= $$HOME/build/llvm

# Setting some variables and commands for compilaten
LLVM_BUILD_PATH = $(LLVM_SRC_PATH)/Release
LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/bin
LLVM_INCLUDES = -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include

CXX = g++
CXXFLAGS_LLVM = -fno-rtti -O0 $(LLVM_INCLUDES)

LLVM_CONFIG_COMMAND = \
		`$(LLVM_BIN_PATH)/llvm-config --cxxflags --libs` \
		`$(LLVM_BIN_PATH)/llvm-config --ldflags`



all: bin/libMackeOpt.so bin/divisible.bc bin/greetings.bc

.PHONY: test
test: all
	@ OPTBIN=$(LLVM_BIN_PATH)/opt python -m unittest

bin/libMackeOpt.so: \
		bin/ListAllFunctions.o \
		bin/ExtractCallgraph.o \
		bin/ChangeEntryPoint.o
	$(CXX) $(CXXFLAGS_LLVM) -shared $(LLVM_CONFIG_COMMAND) $^ -o $@


bin/%.o: lib/%.cpp
	$(CXX) -c -fPIC $(CXXFLAGS_LLVM) $(LLVM_CONFIG_COMMAND) $^ -o $@


bin/%.bc: examples/%.c
	$(LLVM_BIN_PATH)/clang -c -emit-llvm -O0 -g $^ -o $@

.PHONY: clean
clean:
	@ rm -rf bin
	@ git checkout bin/.gitignore

.PHONY: format
format:
	clang-format -i $(wildcard **/*.cpp) $(wildcard **/*.c) $(wildcard **/*.h)
