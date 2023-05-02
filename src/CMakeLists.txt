set(PAUSE_SPOTIFY_TARGET ${CMAKE_PROJECT_NAME})

find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

include(${CMAKE_MODULE_PATH}/CompilerWarnings.cmake)
include(${CMAKE_MODULE_PATH}/ClangTidy.cmake)

add_executable(${PAUSE_SPOTIFY_TARGET})
target_sources(${PAUSE_SPOTIFY_TARGET} PRIVATE
	main.cpp
	application.hpp application.cpp
	app_config.hpp app_config.cpp
	new_audio_session_notifier.hpp new_audio_session_notifier.cpp
	nonspotify_audio_session_event_notifier.hpp nonspotify_audio_session_event_notifier.cpp
	audio_session_controller.hpp audio_session_controller.cpp
	audio_session_list.hpp audio_session_list.cpp
	audio_session_manager.hpp audio_session_manager.cpp
	default_audio_rendering_endpoint.hpp default_audio_rendering_endpoint.cpp
	spotify_app.hpp spotify_app.cpp
	misc.hpp misc.cpp
	logger.hpp logger.cpp
	com_exception.hpp com_exception.cpp
)

target_link_libraries(${PAUSE_SPOTIFY_TARGET} PRIVATE spdlog::spdlog CURL::libcurl Boost::program_options)
target_compile_options(${PAUSE_SPOTIFY_TARGET} PRIVATE ${COMPILER_WARNINGS} ${WARNING_AS_ERROR})
set_target_properties(${PAUSE_SPOTIFY_TARGET} PROPERTIES
	CXX_STANDARD 23
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<IF:$<BOOL:${BUILD_STATIC_LIBS}>,,DLL>"
)

if (ENABLE_CLANG_TIDY)
	message(STATUS "Enable clang-tidy")
	enable_clang_tidy_for_target(${PAUSE_SPOTIFY_TARGET})
endif()
