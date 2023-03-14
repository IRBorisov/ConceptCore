cmake_minimum_required(VERSION 3.15)

add_library(RSLang STATIC IMPORTED)
add_library(cclLang STATIC IMPORTED)
add_library(cclGraph STATIC IMPORTED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(LIB_PREFIX "")
	set(LIB_SUFFIX lib)
else()
	set(LIB_PREFIX lib)
	set(LIB_SUFFIX a)
endif()

set_target_properties(RSLang PROPERTIES 
	IMPORTED_LOCATION_DEBUG ${CMAKE_SOURCE_DIR}/../output/lib/${LIB_PREFIX}RSlangd.${LIB_SUFFIX}
	IMPORTED_LOCATION_RELEASE ${CMAKE_SOURCE_DIR}/../output/lib//${LIB_PREFIX}RSlang.${LIB_SUFFIX}
)
set_target_properties(cclLang PROPERTIES 
	IMPORTED_LOCATION_DEBUG ${CMAKE_SOURCE_DIR}/../output/lib//${LIB_PREFIX}cclLangd.${LIB_SUFFIX}
	IMPORTED_LOCATION_RELEASE ${CMAKE_SOURCE_DIR}/../output/lib//${LIB_PREFIX}cclLang.${LIB_SUFFIX}
)
set_target_properties(cclGraph PROPERTIES 
	IMPORTED_LOCATION_DEBUG ${CMAKE_SOURCE_DIR}/../output/lib//${LIB_PREFIX}cclGraphd.${LIB_SUFFIX}
	IMPORTED_LOCATION_RELEASE ${CMAKE_SOURCE_DIR}/../output/lib//${LIB_PREFIX}cclGraph.${LIB_SUFFIX}
)