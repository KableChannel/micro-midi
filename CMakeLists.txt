cmake_minimum_required(VERSION 3.10..4.1)

project(
	micro-midi
	VERSION 1.0
	DESCRIPTION "An ultra-simple MIDI library focused on the KISS principles."
	LANGUAGES C
)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/Wall /WX /wd4710 /wd4711 /wd5045 /wd4820)
	set_directory_properties(PROPERTIES COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)
	add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function)
endif()

option(BUILD_SHARED_LIBS "Turn on if you want to build dynamic libraries." OFF)
if(BUILD_SHARED_LIBS)
add_compile_definitions(BUILD_SHARED_LIBS)
endif()

add_library(micro-midi "um.h" "um.c")
if (MSVC)
	target_link_libraries(micro-midi PRIVATE Winmm)
elseif (APPLE)
	# TODO: figure out what APPLE uses
endif ()

target_compile_features(micro-midi PRIVATE c_std_99)
set_target_properties(micro-midi PROPERTIES CMAKE_C_STANDARD_REQUIRED True)
set_target_properties(micro-midi PROPERTIES LINKER_LANGUAGE C)
