cmake_minimum_required(VERSION 3.15)

project(EVA_ECS_Test VERSION 1.0 LANGUAGES CXX)

file(GLOB_RECURSE TESTSOURCES
	source/*
)
add_executable(EVA_ECS_Test ${TESTSOURCES})

set_target_properties(
    EVA_ECS_Test
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_subdirectory(dependencies/test)
set(EVA_Test_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
target_link_libraries(EVA_ECS_Test PUBLIC EVA_ECS EVA_Test)
