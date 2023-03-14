cmake_minimum_required(VERSION 3.15)

project (pyconcept VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## Project options
##
option(CC_UseSanitizers "Use sanitizers" FALSE)

# Compiler options
include(cmake/CXXTargets.cmake)

# Libraries folder
link_directories(import/lib)

# Add pybind11
add_subdirectory(pybind11)

##
## Project Setup
##
pybind11_add_module(pyconcept MODULE src/pyconcept.cpp)

target_include_directories(pyconcept
	PUBLIC
		include
	PRIVATE
		header
		import/include
)

target_link_libraries(pyconcept
	PUBLIC
		ConceptCoreLibrary
		RSlang
		cclLang
		cclGraph
	PRIVATE
		ccl_CXXwarnings
		ccl_CXXoptions
)

install(TARGETS pyconcept 
	ARCHIVE
		DESTINATION lib
	LIBRARY
		DESTINATION lib
)