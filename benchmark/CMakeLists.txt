cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  ${CMAKE_PROJECT_NAME}_Benchmarks
  LANGUAGES CXX
)

#
# Set the sources for the unit tests and add the executable(s)
#

file(GLOB benchmark_sources 
  src/**
)
add_executable(${PROJECT_NAME} ${benchmark_sources})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_PROJECT_NAME})

#
# Set the compiler standard
#

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(BENCHMARK_ARCHITECTURE_64)
	add_compile_definitions(BENCHMARK_ARCHITECTURE="x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_compile_definitions(BENCHMARK_ARCHITECTURE_64)
	add_compile_definitions(BENCHMARK_ARCHITECTURE="x86")
else()
    add_compile_definitions(BENCHMARK_ARCHITECTURE_UNKNOWN)
	add_compile_definitions(BENCHMARK_ARCHITECTURE="unknown")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	add_compile_definitions(BENCHMARK_DEBUG)
	add_compile_definitions(BENCHMARK_CONFIGURATION="debug")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
	add_compile_definitions(BENCHMARK_RELEASE)
	add_compile_definitions(BENCHMARK_CONFIGURATION="release")
else()
	add_compile_definitions(BENCHMARK_UNKNOWN)
	add_compile_definitions(BENCHMARK_CONFIGURATION="unknown")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_compile_definitions(BENCHMARK_PLATFORM_WINDOWS)
	add_compile_definitions(BENCHMARK_PLATFORM="windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_compile_definitions(BENCHMARK_PLATFORM_LINUX)
	add_compile_definitions(BENCHMARK_PLATFORM="linux")
els()
	add_compile_definitions(BENCHMARK_PLATFORM_UNKNOWN)
	add_compile_definitions(BENCHMARK_PLATFORM="unknown")
endif()