cmake_minimum_required(VERSION 3.10)

project(pickup_and_delivery)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PARALLEL_EXECUTION "Run iterations in parallel, utilizing multiple threads" ON)
if(PARALLEL_EXECUTION)
    add_compile_definitions(PARALLEL_EXECUTION)
endif()

option(ALL_ALGORITHMS "Run all searching algorithms per file" ON)
if(ALL_ALGORITHMS)
    add_compile_definitions(ALL_ALGORITHMS)
endif()

option(FILE_OUTPUT "Write output to a results file" ON)
if(FILE_OUTPUT)
    add_compile_definitions(FILE_OUTPUT)
endif()

option(RUN_FOR_10_MINUTES "Run the last file for all remaining execution time" ON)
if (RUN_FOR_10_MINUTES)
    add_compile_definitions(RUN_FOR_10_MINUTES)
endif()



add_subdirectory(src)