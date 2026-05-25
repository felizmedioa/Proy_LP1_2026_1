cmake_minimum_required(VERSION 3.22.1)

project("expressionevaluator")

# Add compiler definition to intercept printf calls globally
add_compile_definitions(printf=my_printf)

add_library(
    # Sets the name of the library.
    expression_evaluator

    # Sets the library as a shared library.
    SHARED

    # Provides a relative path to your source file(s).
    native-lib.c
    ../../../../../eval.c
    ../../../../../functions.c
    ../../../../../lexer.c
    ../../../../../parser.c
    ../../../../../symtable.c
)

# Include directories
target_include_directories(
    expression_evaluator PRIVATE
    ../../../../..
)

find_library(
    # Sets the name of the path variable.
    log-lib

    # Specifies the name of the NDK library that
    # you want CMake to locate.
    log
)

target_link_libraries(
    # Specifies the target library.
    expression_evaluator

    # Links the target library to the log library
    # included in the NDK.
    ${log-lib}
    m # Link math library
)
