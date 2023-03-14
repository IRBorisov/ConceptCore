cmake_minimum_required(VERSION 3.15)

find_package(GTest REQUIRED)

add_executable(Tests)
target_sources(Tests 
	PRIVATE 
		unity/cclLangTest.cpp
)
target_include_directories(Tests 
	PRIVATE 
		../header
		../import/include
		utils
)
target_link_libraries(Tests
	PRIVATE
		cclLang
		ccl_CXXwarnings
		ccl_CXXoptions
		GTest::GTest
		GTest::Main
)

include(GoogleTest)
gtest_discover_tests(Tests)