set(PAUSE_SPOTIFY_TARGET ${CMAKE_PROJECT_NAME})

# Need to do this while building with VCPKG_TARGET_TRIPLET=x64-windows-static
string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

if (MSVC)
	# get rid of warning "warning D9025: overriding '/W3' with '/W4'"
	# from some reason policy CMP0092 does not apply
	string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

add_executable(${PAUSE_SPOTIFY_TARGET})
target_sources(${PAUSE_SPOTIFY_TARGET} PRIVATE main.cpp
											   pause_spotify_app.hpp pause_spotify_app.cpp
				                               app_options.hpp app_options.cpp
				                               new_audio_session_notifier.hpp new_audio_session_notifier.cpp
				                               nonspotify_audio_session_event_notifier.hpp nonspotify_audio_session_event_notifier.cpp
				                               audio_session_controller.hpp audio_session_controller.cpp
				                               audio_session_list.hpp audio_session_list.cpp
				                               audio_session_manager.hpp audio_session_manager.cpp
				                               default_audio_rendering_endpoint.hpp default_audio_rendering_endpoint.cpp
				                               spotify_app.hpp spotify_app.cpp
				                               misc.hpp misc.cpp
											   logger.hpp logger.cpp)

target_link_libraries(${PAUSE_SPOTIFY_TARGET} PRIVATE spdlog::spdlog CURL::libcurl Boost::program_options)
target_compile_definitions(${PAUSE_SPOTIFY_TARGET} PRIVATE $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=1>)
target_compile_options(${PAUSE_SPOTIFY_TARGET} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>)
set_target_properties(${PAUSE_SPOTIFY_TARGET} PROPERTIES CXX_STANDARD 20)
set_target_properties(${PAUSE_SPOTIFY_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")