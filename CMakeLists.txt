cmake_minimum_required(VERSION 3.2)

project("PauseSpotify")

if(NOT WIN32)
	message(FATAL_ERROR "Platform not supported")
endif()

# With "Visual Studio" CMake generator, pass only if compiler is set to ClangCl
if(MSVC AND (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	message(FATAL_ERROR "Cannot build with MSVC due to the lack of support for statement expression used with std::expected")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/release/bin)

option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

add_subdirectory("src")
