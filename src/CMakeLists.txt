set(RESUME_SPOTIFY_TARGET ${CMAKE_PROJECT_NAME})

set(SOURCE_FILES main.cpp
				 pause_spotify_app.cpp
				 app_options.cpp
				 new_audio_session_notifier.cpp
				 nonspotify_audio_session_event_notifier.cpp
				 audio_session_controller.cpp
				 audio_session_list.cpp
				 audio_session_manager.cpp
				 default_audio_rendering_endpoint.cpp
				 spotify_app.cpp
				 misc.cpp
				 )

find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

if (MSVC)
	# get rid of warning "warning D9025: overriding '/W3' with '/W4'"
	# from some reason policy CMP0092 does not apply
	string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

add_executable(${RESUME_SPOTIFY_TARGET} ${SOURCE_FILES})
target_link_libraries(${RESUME_SPOTIFY_TARGET} PRIVATE spdlog::spdlog CURL::libcurl Boost::program_options)
target_compile_definitions(${RESUME_SPOTIFY_TARGET} PRIVATE $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=1>)
target_compile_options(${RESUME_SPOTIFY_TARGET} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>)
set_target_properties(${RESUME_SPOTIFY_TARGET} PROPERTIES CXX_STANDARD 17)
set_target_properties(${RESUME_SPOTIFY_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")