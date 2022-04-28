cmake_minimum_required(VERSION 3.2)

project("pause_spotify")

if(NOT WIN32)
	message(FATAL_ERROR "Platform not supported")
endif()

add_subdirectory("src")