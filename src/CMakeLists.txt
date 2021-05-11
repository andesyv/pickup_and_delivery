add_executable(pickup_and_delivery main.cpp)
target_sources(pickup_and_delivery
    PRIVATE problem.cpp
    PRIVATE heuristics.cpp
    PRIVATE cost.cpp
    PRIVATE feasibility.cpp
    PRIVATE operators.cpp
)
if (GPU_ACCELERATION)
    target_sources(pickup_and_delivery PRIVATE window.cpp)
endif()

add_executable(solution_check solutioncheck.cpp)
target_sources(solution_check PRIVATE problem.cpp cost.cpp feasibility.cpp)

add_subdirectory(data)