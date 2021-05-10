if (GPU_ACCELERATION)
    # GLFW:
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
    add_subdirectory(glfw)
    target_include_directories(pickup_and_delivery PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
    target_link_libraries(pickup_and_delivery PRIVATE glfw)

    if (WIN32)
        target_link_libraries(pickup_and_delivery PRIVATE opengl32.lib)
    endif()

    # GLM:
    target_include_directories(pickup_and_delivery PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glm)
endif()