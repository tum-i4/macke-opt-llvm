# Load LLVM with all includes
find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Load all subdirectories and combine them into one library
add_subdirectory(ListAllFunctions)
#add_subdirectory(ExtractCallgraph)
add_library(MackeOpt MODULE
    $<TARGET_OBJECTS:ListAllFunctions>
#    $<TARGET_OBJECTS:ExtractCallgraph>
)

# set_property(TARGET MackeOpt PROPERTY CXX_STANDARD 98)
