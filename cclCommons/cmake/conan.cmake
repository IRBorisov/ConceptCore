cmake_minimum_required(VERSION 3.14)

##
## Conan setup using cmake-conan
## https://github.com/conan-io/cmake-conan
##
if(NOT EXISTS "${PROJECT_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(DOWNLOAD 
		"https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake"
		"${PROJECT_BINARY_DIR}/conan.cmake"
	)
endif()

include(${PROJECT_BINARY_DIR}/conan.cmake)
conan_check(REQUIRED)
conan_cmake_run(
	CONANFILE conanfile.txt
	BASIC_SETUP CMAKE_TARGETS
	BUILD missing
)