cmake_minimum_required(VERSION 3.23)

project (cclCommons VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## Project options
##
option(CC_BuildTests "Include tests executable" TRUE)
option(CC_UseSanitizers "Use sanitizers" FALSE)

## Compiler options
include(../cmake/CXXTargets.cmake)

##
## Project Setup
##
install(DIRECTORY include/ DESTINATION include)

if(CC_BuildTests)
  enable_testing()
  add_subdirectory("test")
endif()