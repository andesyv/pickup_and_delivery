add_executable(pickup_and_delivery main.cpp)
target_sources(pickup_and_delivery
    PRIVATE problem.cpp
    PRIVATE heuristics.cpp
    PRIVATE cost.cpp
    PRIVATE feasibility.cpp
    PRIVATE operators.cpp
)
if (GPU_ACCELERATION)
    target_sources(pickup_and_delivery PRIVATE shader.cpp)

    # Shader code parsing into shader.cpp file:
    file(GLOB SHADERFILES "*.glsl" "*.vs" "*.gs" "*.fs")
    foreach(shaderfile IN LISTS SHADERFILES)
        # Need CMake 3.20 for cmake_path
        cmake_path(GET shaderfile FILENAME varName)
        file(READ ${shaderfile} ${varName})
    endforeach()

    configure_file(window.cpp window.gen.cpp)
    configure_file(window.h window.h COPYONLY)
    configure_file(shader.h shader.h COPYONLY)
    target_sources(pickup_and_delivery PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/window.gen.cpp
    )
endif()

add_executable(solution_check solutioncheck.cpp)
target_sources(solution_check PRIVATE problem.cpp cost.cpp feasibility.cpp)

add_subdirectory(data)