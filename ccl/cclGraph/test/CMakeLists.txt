cmake_minimum_required(VERSION 3.23)

find_package(GTest REQUIRED)

add_executable(cclGraph_Tests)
target_sources(cclGraph_Tests 
  PRIVATE 
    src/testConnectionsGraph.cpp
)
target_include_directories(cclGraph_Tests 
  PRIVATE 
    ../header
    ../import/include
    utils
)
target_link_libraries(cclGraph_Tests
  PRIVATE
    cclGraph
    ccl_CXXwarnings
    ccl_CXXoptions
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cclGraph_Tests)