target_sources(pickup_and_delivery PRIVATE main.cpp problem.cpp heuristics.cpp cost.cpp feasibility.cpp operators.cpp)

add_executable(solution_check solutioncheck.cpp)
target_sources(solution_check PRIVATE problem.cpp cost.cpp feasibility.cpp)

add_subdirectory(data)