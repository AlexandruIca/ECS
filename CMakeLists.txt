cmake_minimum_required(VERSION 3.10)

project(ecs LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

add_library(project_options INTERFACE)
add_library(project::options ALIAS project_options)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
add_library(project::warnings ALIAS project_warnings)
set_project_warnings(project_warnings)

enable_sanitizers(project_options)

# Not to be used directly by users, just storing the include directory that will be specified in:
# `target_include_directories(ecs PUBLIC ${ECS_INCLUDE_DIR})`
set(ECS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)

option(BUILD_TESTS "Build the tests that come with `ecs`" OFF)

if(BUILD_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)
endif()
