cmake_minimum_required(VERSION 3.14)

project (cclLang VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## Project options
##
option(CC_BuildTests "Include tests executable" TRUE)
option(CC_UseSanitizers "Use sanitizers" FALSE)

## Compiler options
include(cmake/CXXTargets.cmake)

## Enable Conan
include(cmake/conan.cmake)

##
## Project Setup
##
add_library(cclLang)

target_sources(cclLang
	PRIVATE
		unity/cclLang.cpp
)
target_include_directories(cclLang
	PUBLIC
		../cclCommons/include
		include
	PRIVATE
		header
		import/include
)
target_link_libraries(cclLang
	PRIVATE 
		ccl_CXXwarnings
		ccl_CXXoptions
)
set_target_properties(cclLang PROPERTIES 
	OUTPUT_NAME cclLang
	DEBUG_POSTFIX d
)
if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set_target_properties(cclLang PROPERTIES
			COMPILE_PDB_NAME cclLang)
	else()
		set_target_properties(cclLang PROPERTIES
			COMPILE_PDB_NAME cclLangd)
	endif()
endif()

install(TARGETS cclLang 
	ARCHIVE
		DESTINATION lib
	LIBRARY
		DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)

if(CC_BuildTests)
	enable_testing()
	add_subdirectory("test")
endif()