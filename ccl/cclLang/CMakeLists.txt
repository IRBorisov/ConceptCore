cmake_minimum_required(VERSION 3.23)

project (cclLang VERSION 1.2.0 LANGUAGES CXX)

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
add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES 
  OUTPUT_NAME ${PROJECT_NAME}
  DEBUG_POSTFIX d
  POSITION_INDEPENDENT_CODE ON
)

target_sources(${PROJECT_NAME}
  PRIVATE
    unity/cclLang.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../cclCommons/include
    include
  PRIVATE
    header
    import/include
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE 
    ccl_CXXwarnings
    ccl_CXXoptions
)

if(MSVC)
  if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(LIB_SUFFIX d)
  else()
    set(LIB_SUFFIX "")
  endif ()
  
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
      COMPILE_PDB_NAME ${PROJECT_NAME}${LIB_SUFFIX}
  )

  get_target_property(output_dir ${PROJECT_NAME} BINARY_DIR)
  install(FILES ${output_dir}/$<CONFIG>/${PROJECT_NAME}${LIB_SUFFIX}.pdb DESTINATION lib)
endif()

install(TARGETS ${PROJECT_NAME} 
  ARCHIVE
    DESTINATION lib
  LIBRARY
    DESTINATION lib
  INCLUDES 
    DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)

if(CC_BuildTests)
  enable_testing()
  add_subdirectory("test")
endif()