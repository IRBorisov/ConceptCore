cmake_minimum_required(VERSION 3.23)

project (ConceptCoreLibrary VERSION 1.2.0 LANGUAGES CXX)

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
    core/unity/CCL.cpp
    cclGraph/src/CGraph.cpp
    rslang/unity/reflex_unity1.cpp
    rslang/unity/reflex_unity2.cpp
    rslang/unity/RSlang.cpp
    rslang/unity/RSlang2.cpp
    cclLang/unity/cclLang.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    cclCommons/include
    cclGraph/include
    cclLang/include
    rslang/include
    core/include
  PRIVATE
    core/import/include
    core/header
    cclGraph/header
    cclGraph/import/include
    rslang/header
    rslang/import/include
    rslang/import/reflex/include
    cclLang/header
    cclLang/import/include
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
install(DIRECTORY core/include/ DESTINATION include)

if(CC_BuildTests)
  add_subdirectory(cclCommons)
  add_subdirectory(cclGraph)
  add_subdirectory(cclLang)
  add_subdirectory(rslang)

  enable_testing()
  add_subdirectory("core/test")
endif()