cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  ${CMAKE_PROJECT_NAME}Tests
  LANGUAGES CXX
)

verbose_message("Adding tests under ${CMAKE_PROJECT_NAME}Tests...")

#
# Set the sources for the unit tests and add the executable(s)
#

set(test_sources 
  src/math_test.cpp
)
add_executable(${PROJECT_NAME} ${test_sources})

#
# Set the compiler standard
#

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#
# Setup code coverage if enabled
#

if (${CMAKE_PROJECT_NAME}_ENABLE_CODE_COVERAGE)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(${CMAKE_PROJECT_NAME} PUBLIC -fprofile-arcs -ftest-coverage)
  verbose_message("Code coverage is enabled and provided with GCC.")
endif()

#
# Load the desired unit testing framework
#
# Currently supported: GoogleTest (and GoogleMock), Catch2.

if(${CMAKE_PROJECT_NAME}_BUILD_EXECUTABLE)
	set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME}_LIB)
else()
	set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME})
endif()

if(${CMAKE_PROJECT_NAME}_USE_GTEST)

  #find_package(GTest REQUIRED)
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()
  
  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  
  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)
  
  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()


  if(${CMAKE_PROJECT_NAME}_USE_GOOGLE_MOCK)
		set(GOOGLE_MOCK_LIBRARIES GTest::gmock GTest::gmock_main)
  endif()

  add_compile_definitions(GTEST_HAS_PTHREAD=0)

  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
      gtest_main
      ${GOOGLE_MOCK_LIBRARIES}
      ${${CMAKE_PROJECT_NAME}_TEST_LIB}
  )
elseif(${CMAKE_PROJECT_NAME}_USE_CATCH2)
  find_package(Catch2 REQUIRED)
  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
      Catch2::Catch2
      ${${CMAKE_PROJECT_NAME}_TEST_LIB}
  )
else()
  message(FATAL_ERROR "Unknown testing library. Please setup your desired unit testing library by using `target_link_libraries`.")  
endif()

#
# Add the unit tests
#

add_test(
  NAME
    ${PROJECT_NAME}
  COMMAND
    ${PROJECT_NAME}
)

verbose_message("Finished adding unit tests for ${CMAKE_PROJECT_NAME}.")
