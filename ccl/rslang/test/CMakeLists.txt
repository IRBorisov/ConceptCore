cmake_minimum_required(VERSION 3.23)

find_package(GTest REQUIRED)

add_executable(rslang_Tests)
target_sources(rslang_Tests 
  PRIVATE 
    unity/rslTest.cpp
)
target_include_directories(rslang_Tests 
  PRIVATE 
    ../header
    ../import/include
    utils
)
target_link_libraries(rslang_Tests
  PRIVATE
    RSlang
    ccl_CXXwarnings
    ccl_CXXoptions
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(rslang_Tests)