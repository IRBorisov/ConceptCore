cmake_minimum_required(VERSION 3.23)

find_package(GTest REQUIRED)

add_executable(cclLang_Tests)
target_sources(cclLang_Tests 
  PRIVATE 
    unity/cclLangTest.cpp
)
target_include_directories(cclLang_Tests 
  PRIVATE 
    ../header
    ../import/include
    utils
)
target_link_libraries(cclLang_Tests
  PRIVATE
    cclLang
    ccl_CXXwarnings
    ccl_CXXoptions
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cclLang_Tests)