cmake_minimum_required(VERSION 3.23)

##
## Compiler options
##
add_library(ccl_CXXwarnings INTERFACE)
add_library(ccl_CXXoptions INTERFACE)

target_compile_features(ccl_CXXoptions INTERFACE cxx_std_20)

if(CC_UseSanitizers AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(ccl_CXXoptions
    INTERFACE 
      -fsanitize=address
      -fsanitize=undefined
  )
  target_link_libraries(ccl_CXXoptions
    INTERFACE 
      -fsanitize=address
      -fsanitize=undefined
  )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(ccl_CXXwarnings
    INTERFACE
      -Wall
      -Werror
      -Wextra
      -Wextra-semi-stmt
      -Wshadow
      -Wconversion
      -Wold-style-cast
      -Wunused
      -Woverloaded-virtual
      -Wnull-dereference
      -Wdouble-promotion
      -Wno-invalid-token-paste
      -Wno-unknown-pragmas
      -Wno-unused-variable
      -Wformat=2
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(ccl_CXXwarnings
    INTERFACE
      -Wall
      -Werror
      -Wmisleading-indentation
      -Wduplicated-cond
      -Wno-unknown-pragmas
      -Wduplicated-branches
      -Wuseless-cast
      -Wlogical-op
      -fexceptions
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(ccl_CXXoptions
    INTERFACE 
      /Zi
  )
  target_compile_options(ccl_CXXwarnings
    INTERFACE
      /permissive- 
      /W4
  )
endif()