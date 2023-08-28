set(PAUSE_SPOTIFY_TARGET ${CMAKE_PROJECT_NAME})

find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

include(${CMAKE_MODULE_PATH}/CompilerWarnings.cmake)
include(${CMAKE_MODULE_PATH}/ClangTidy.cmake)

add_executable(${PAUSE_SPOTIFY_TARGET})
target_sources(
	${PAUSE_SPOTIFY_TARGET}
	PRIVATE
	main.cpp
	application.hpp application.cpp
	app_config.hpp app_config.cpp
	new_audio_session_notifier.hpp new_audio_session_notifier.cpp
	nonspotify_audio_session_event_notifier.hpp nonspotify_audio_session_event_notifier.cpp
	audio_session.hpp audio_session.cpp
	audio_session_list.hpp audio_session_list.cpp
	audio_session_manager.hpp audio_session_manager.cpp
	audio_output_device.hpp audio_output_device.cpp
	spotify_app.hpp spotify_app.cpp
	misc.hpp
	logger.hpp logger.cpp
	windows_error.hpp windows_error.cpp
	com_error.hpp com_error.cpp
	com_ptr.hpp
	windows_utils.hpp windows_utils.cpp
)

target_link_libraries(
	${PAUSE_SPOTIFY_TARGET}
	PRIVATE
	spdlog::spdlog
	CURL::libcurl
	Boost::program_options
)
target_compile_options(
	${PAUSE_SPOTIFY_TARGET}
	PRIVATE
	${COMPILER_WARNINGS}
	${WARNING_AS_ERROR}
	-Wno-gnu-statement-expression
)
target_compile_definitions(
	${PAUSE_SPOTIFY_TARGET}
	PRIVATE
	PROJECT_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"
)

if (ENABLE_CLANG_TIDY)
	message(STATUS "Enable clang-tidy")
	enable_clang_tidy_for_target(${PAUSE_SPOTIFY_TARGET})
endif()
