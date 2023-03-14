cmake_minimum_required(VERSION 3.15)

##
## Conan setup using cmake-conan
## https://github.com/conan-io/cmake-conan
##
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(DOWNLOAD 
			"https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
			"${CMAKE_BINARY_DIR}/conan.cmake"
			TLS_VERIFY ON
	)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
	REQUIRES
		gtest/1.12.1
	GENERATORS
		CMakeDeps
		CMakeToolchain
)

conan_cmake_autodetect(settings)

conan_cmake_install(
	PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings}
)