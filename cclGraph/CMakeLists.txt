cmake_minimum_required(VERSION 3.15)

project (cclGraph VERSION 1.1.0 LANGUAGES CXX)

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
add_library(cclGraph)
set_target_properties(cclGraph PROPERTIES 
	OUTPUT_NAME cclGraph
	DEBUG_POSTFIX d
	POSITION_INDEPENDENT_CODE ON
)

target_sources(cclGraph
	PRIVATE
		src/CGraph.cpp
)
target_include_directories(cclGraph
	PUBLIC
		../cclCommons/include
		include
	PRIVATE
		import/include
		header
)
target_link_libraries(cclGraph
	PRIVATE 
		ccl_CXXwarnings
		ccl_CXXoptions
)
if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set_target_properties(cclGraph PROPERTIES
			COMPILE_PDB_NAME cclGraph)
	else()
		set_target_properties(cclGraph PROPERTIES
			COMPILE_PDB_NAME cclGraphd)
	endif()
endif()

install(TARGETS cclGraph 
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