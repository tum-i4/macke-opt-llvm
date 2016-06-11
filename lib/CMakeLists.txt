# Load LLVM with all includes
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Load all subdirectories and combine them into one library
add_subdirectory(ListAllFunctions)
add_library(MackeOpt MODULE $<TARGET_OBJECTS:ListAllFunctions>)
