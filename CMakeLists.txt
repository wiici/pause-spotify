cmake_minimum_required(VERSION 3.2)

# Set this policy to be able to properly use MSVC_RUNTIME_LIBRARY
cmake_policy(SET CMP0091 NEW)

project("pause_spotify")

if(NOT WIN32)
	message(FATAL_ERROR "Platform not supported")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory("src")