#
# Output variables:
# COMPILER_WARNINGS
# WARNING_AS_ERROR
#

if (MSVC)
    set(COMPILER_WARNINGS
            /Wall

            /wd4710
            /wd4711
            /wd5045
            /wd4820
            /wd4626
    )

    set(WARNING_AS_ERROR /WX)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_WARNINGS
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough

        -Wno-language-extension-token
    )

    set(WARNING_AS_ERROR -Werror)

endif()