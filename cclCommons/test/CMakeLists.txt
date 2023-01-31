cmake_minimum_required(VERSION 3.14)

find_package(GTest REQUIRED)

add_executable(Tests)
target_sources(Tests 
	PRIVATE 
		unity/cclCommonsTest.cpp
)
target_include_directories(Tests 
	PRIVATE 
		../include
		utils
)
target_link_libraries(Tests
	PRIVATE
		ccl_CXXwarnings
		ccl_CXXoptions
		GTest::GTest
		GTest::Main
)

include(GoogleTest)
gtest_discover_tests(Tests)