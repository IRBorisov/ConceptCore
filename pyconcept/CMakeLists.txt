cmake_minimum_required(VERSION 3.23)

project (pyconcept VERSION 1.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##
## Project options
##
option(CC_UseSanitizers "Use sanitizers" FALSE)
option(CC_BuildTests "Include tests executable" FALSE)

# Compiler options
include(cmake/CXXTargets.cmake)

## Import internal targets
add_subdirectory(ccl)

# Add pybind11
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)

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
  PRIVATE
    pybind11_all_do_not_use
    ccl_CXXwarnings
    ccl_CXXoptions
)

install(TARGETS pyconcept 
  ARCHIVE
    DESTINATION lib
  LIBRARY
    DESTINATION lib
)