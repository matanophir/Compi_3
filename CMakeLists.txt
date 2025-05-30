cmake_minimum_required(VERSION 3.16)
project(SymTableTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug mode
set(CMAKE_BUILD_TYPE Debug)

# Set debug flags for optimal debugging experience
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")

# Only include the necessary source files for test_symtable
set(TEST_SOURCES
    test_symtable.cpp
    nodes.cpp
    output.cpp
    symtable.cpp
)

# Create the test executable
add_executable(test_symtable ${TEST_SOURCES})

# Set compiler options for debug mode
target_compile_options(test_symtable PRIVATE 
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -DDEBUG>
)

# Enable debugging symbols
set_target_properties(test_symtable PROPERTIES
    COMPILE_FLAGS "-g"
    LINK_FLAGS "-g"
)

# Custom target to run the test
add_custom_target(run_test
    COMMAND test_symtable
    DEPENDS test_symtable
    COMMENT "Running SymTable tests"
    VERBATIM
)

# Custom target to run with GDB
add_custom_target(debug_test
    COMMAND gdb -ex run --args test_symtable
    DEPENDS test_symtable
    COMMENT "Running SymTable tests in GDB"
    VERBATIM
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Sources: ${TEST_SOURCES}")
