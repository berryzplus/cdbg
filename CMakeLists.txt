cmake_minimum_required(VERSION 3.7)

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

if(BUILD_EXAMPLES)
  list(APPEND VCPKG_MANIFEST_FEATURES "examples")
endif()

# define a project
project(cdbg)

# define a variable SOURCES with file GLOB
file(GLOB SOURCES dbgstream.cpp)
file(GLOB HEADERS dbgstream.h)

# add library
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# set C++ Standard Level
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# set library include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cdbg>  # <prefix>/include/cdbg
)

# define installation of headers
install(FILES ${HEADERS} DESTINATION "include/cdbg")

# define installation of archive
install(
  TARGETS cdbg
  EXPORT  cdbg-targets
  ARCHIVE DESTINATION lib
)

if(BUILD_TESTING)
  # define a variable for test-suite's name
  set(TEST_SUITE_NAME cdbgtest)

  # define a variable SOURCES with file GLOB
  file(GLOB TEST_SOURCES test/debug_stream_test.cpp tools/CrtAllocHook/CrtAllocHook.hpp test/coverage.cpp)

  # add test executable
  add_executable(${TEST_SUITE_NAME} ${TEST_SOURCES})

  # add definitions
  target_compile_definitions(${TEST_SUITE_NAME} PUBLIC _CONSOLE UNICODE _UNICODE _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)

  # set C++ Standard Level
  set_target_properties(${TEST_SUITE_NAME} PROPERTIES CXX_STANDARD 17)

  # find GTest
  find_package(GTest CONFIG REQUIRED)

  # define dependencies
  target_link_libraries(${TEST_SUITE_NAME}
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main
    PRIVATE GTest::gmock
    PRIVATE ${PROJECT_NAME})

  # include current directory
  target_include_directories(${TEST_SUITE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tools/CrtAllocHook)

  # enable testing
  include(CTest)
  enable_testing()

  # declare test command
  add_test(NAME test COMMAND ${TEST_SUITE_NAME})
endif()

if(BUILD_EXAMPLES)
  # define a variable for example's name
  set(EXAMPLE_NAME example)

  # define a variable SOURCES with file GLOB
  file(GLOB SOURCES example.cpp)

  # add example executable
  add_executable(${EXAMPLE_NAME} ${SOURCES})

  # set C++ Standard Level
  set_target_properties(${EXAMPLE_NAME} PROPERTIES CXX_STANDARD 17)

  # add dependencies
  target_link_libraries(${EXAMPLE_NAME}
    PRIVATE ${PROJECT_NAME})
endif()
