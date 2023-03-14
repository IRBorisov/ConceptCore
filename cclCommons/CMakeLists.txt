cmake_minimum_required(VERSION 3.15)

project (cclCommons VERSION 1.1.0 LANGUAGES CXX)

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

install(DIRECTORY include/ DESTINATION include)

if(CC_BuildTests)
	enable_testing()
	add_subdirectory("test")
endif()