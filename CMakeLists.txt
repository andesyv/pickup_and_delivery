cmake_minimum_required(VERSION 3.13)

project(pickup_and_delivery)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

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


add_subdirectory(src)


option(GPU_ACCELERATION "Build with OpenGL, GLFW and GLM in order to utilize GPU algorithms" ON)
if (GPU_ACCELERATION)
    # Init submodules if they aren't inited yet:
    find_package(Git QUIET)

    if(NOT GIT_FOUND)
        message(FATAL_ERROR "Git not found, cannot init submodules.")
    else()
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()

    # Add library subdirectory cmakelists.txt:
    add_subdirectory(lib)

    # Add a define to the c++ code:
    add_compile_definitions(GPU_ACCELERATION)
endif()