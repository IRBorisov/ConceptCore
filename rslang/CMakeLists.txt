cmake_minimum_required(VERSION 3.14)

project (RSlang VERSION 1.1.0 LANGUAGES CXX)

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
add_library(RSlang)

target_sources(RSlang
	PRIVATE
		unity/reflex_unity1.cpp
		unity/reflex_unity2.cpp
		unity/RSlang.cpp
		unity/RSlang2.cpp
)
target_include_directories(RSlang
	PUBLIC
		../cclCommons/include
		include
	PRIVATE
		header
		import/include
		import/reflex/include
)
target_link_libraries(RSlang
	PRIVATE 
		ccl_CXXwarnings
		ccl_CXXoptions
)
set_target_properties(RSlang PROPERTIES 
	OUTPUT_NAME RSlang
	DEBUG_POSTFIX d
)
if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set_target_properties(RSlang PROPERTIES
			COMPILE_PDB_NAME RSlang)
	else()
		set_target_properties(RSlang PROPERTIES
			COMPILE_PDB_NAME RSlangd)
	endif()
endif()

install(TARGETS RSlang 
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