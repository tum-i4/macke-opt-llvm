find_package(LLVM REQUIRED CONFIG)

# Create a list of all example files
set(EXAMPLEFILES
  divisible
)


foreach(EXFILE ${EXAMPLEFILES})

  # Create a custom build command for each example file
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${EXFILE}.bc"
    COMMAND clang -c -emit-llvm -g -O0 "${CMAKE_CURRENT_SOURCE_DIR}/${EXFILE}.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building ${EXFILE}.bc"
  )

  # And store the resulting .bc in a dependency list
  list( APPEND deps "${CMAKE_CURRENT_BINARY_DIR}/${EXFILE}.bc" )
endforeach(EXFILE)

# Finally register the dependencies for the ALL target
add_custom_target(examples ALL DEPENDS ${deps})
