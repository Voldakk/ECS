cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EVA_ECS VERSION 1.0 LANGUAGES CXX)

# Source files
file(GLOB_RECURSE SOURCES
	source/*
)

# Create library
add_library(EVA_ECS STATIC ${SOURCES})

set_target_properties(
    EVA_ECS
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Include paths
target_include_directories(EVA_ECS 
	PUBLIC "include"
)

option(${PROJECT_NAME}_ENABLE_TESTS "Enable tests" OFF)
if(${PROJECT_NAME}_ENABLE_TESTS)
  add_subdirectory(test)
endif()

option(${PROJECT_NAME}_ENABLE_BENCHMARKS "Enable benchmarks" OFF)
if(${PROJECT_NAME}_ENABLE_BENCHMARKS)
  add_subdirectory(benchmark)
endif()
