add_executable(pickup_and_delivery main.cpp)
target_sources(pickup_and_delivery PRIVATE problem.cpp heuristics.cpp cost.cpp feasibility.cpp operators.cpp)

add_subdirectory(data)