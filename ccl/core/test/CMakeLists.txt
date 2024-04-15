cmake_minimum_required(VERSION 3.23)

find_package(GTest REQUIRED)

add_executable(ccl_Tests)
target_sources(ccl_Tests 
  PRIVATE
    unity/cclTest.cpp
)
target_include_directories(ccl_Tests 
  PRIVATE
    ../header
    ../import/include
    utils
)
target_link_libraries(ccl_Tests
  PRIVATE
    ConceptCoreLibrary
    ccl_CXXwarnings
    ccl_CXXoptions
    GTest::gtest
    GTest::gtest_main
)
# TODO: use top level project output directory instead
target_link_directories(ccl_Tests
  PRIVATE
    ../../../output/lib
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(ccl_Tests
    PRIVATE
      /bigobj
  )
endif()

include(GoogleTest)
gtest_discover_tests(ccl_Tests)