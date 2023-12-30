cmake_minimum_required(VERSION 3.5)
project(gfs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GFS_BUILD_TESTS "Build gfs's uint tests" ON)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wthread-safety HAVE_CLANG_THREAD_SAFETY)

# Used by googletest.
check_cxx_compiler_flag(-Wno-missing-field-initializers
                        LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)

                        include(CheckCXXSourceCompiles)

# Test whether C++17 __has_include is available.
check_cxx_source_compiles("
#if defined(__has_include) &&  __has_include(<string>)
#include <string>
#endif
int main() { std::string str; return 0; }
" HAVE_CXX17_HAS_INCLUDE)

include_directories(src/util)

add_library(gfs "")
target_sources(gfs
  PRIVATE
    "src/master/namespace_manager.cc"
    "src/master/namespace_manager.h"
)

target_include_directories(gfs
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

if(HAVE_CLANG_THREAD_SAFETY)
  target_compile_options(gfs
    PUBLIC
      -Werror -Wthread-safety)
endif(HAVE_CLANG_THREAD_SAFETY)

find_package(Threads REQUIRED)
target_link_libraries(gfs Threads::Threads)

find_package(Protobuf REQUIRED)
target_link_libraries(gfs ${Protobuf_INCLUDE_DIRS})

find_package(gRPC REQUIRED)
target_link_libraries(gfs gRPC::grpc++)

if(GFS_BUILD_TESTS)
  enable_testing()
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory("third_party/googletest")
  if(LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
  endif(LEVELDB_HAVE_NO_MISSING_FIELD_INITIALIZERS)
  
  add_executable(gfs_tests "")
  target_sources(gfs_tests
    PRIVATE
      "src/master/namespace_manager_test.cc"
  )
  target_link_libraries(gfs_tests gfs gmock gtest gtest_main)
  function(gfs_test test_file)
    get_filename_component(test_target_name "${test_file}" NAME_WE)
    add_executable("${test_target_name}" "")
    target_sources("${test_target_name}"
      PRIVATE
        "${test_file}"
    )
    target_link_libraries("${test_target_name}" gfs gmock gtest gtest_main)
    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
  endfunction(gfs_test)

  gfs_test("src/master/namespace_test.cc")
  
endif(GFS_BUILD_TESTS)