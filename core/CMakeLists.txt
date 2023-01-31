cmake_minimum_required(VERSION 3.14)

project (ConceptCoreLibrary VERSION 1.1.0 LANGUAGES CXX)

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

## Import internal targets
include(cmake/ImportConceptTargets.cmake)

##
## Project Setup
##

add_library(ccl)

target_sources(ccl
	PRIVATE
		unity/CCL.cpp
)
target_include_directories(ccl
	PUBLIC
		../output/include
		include
	PRIVATE
		header
		import/include
)
target_link_libraries(ccl
	PUBLIC
		CONAN_PKG::nlohmann_json
	INTERFACE
		RSLang
		cclLang
		cclGraph
	PRIVATE
		ccl_CXXwarnings
		ccl_CXXoptions
)
set_target_properties(ccl PROPERTIES 
	OUTPUT_NAME ConceptCoreLibrary
	DEBUG_POSTFIX d
)
if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set_target_properties(ccl PROPERTIES
			COMPILE_PDB_NAME ConceptCoreLibrary)
	else()
		set_target_properties(ccl PROPERTIES
			COMPILE_PDB_NAME ConceptCoreLibraryd)
	endif()
endif()

install(TARGETS ccl 
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