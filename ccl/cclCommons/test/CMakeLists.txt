cmake_minimum_required(VERSION 3.23)

find_package(GTest REQUIRED)

add_executable(cclCommons_Tests)
target_sources(cclCommons_Tests 
  PRIVATE 
    unity/cclCommonsTest.cpp
)
target_include_directories(cclCommons_Tests 
  PRIVATE 
    ../include
    utils
)
target_link_libraries(cclCommons_Tests
  PRIVATE
    ccl_CXXwarnings
    ccl_CXXoptions
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cclCommons_Tests)