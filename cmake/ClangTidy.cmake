function(enable_clang_tidy_for_target TARGET_NAME)

  find_program(CLANG_TIDY_EXE "clang-tidy" REQUIRED)

  get_target_property(TARGET_CXX_STANDARD ${TARGET_NAME} CXX_STANDARD)
  # clang-tidy v16.0.1 only supports c++23 as a working draft (using c++2b)
  if (${TARGET_CXX_STANDARD} STREQUAL "23")
    set(TARGET_CXX_STANDARD "2b")
  endif()

  set(CLANG_TIDY_CONFIG_FILE "${CMAKE_SOURCE_DIR}/.clang-tidy.json")
  set(CLANG_TIDY_COMMAND
        ${CLANG_TIDY_EXE}
        "--config-file=${CLANG_TIDY_CONFIG_FILE}"
        "-extra-arg=-std=c++${TARGET_CXX_STANDARD}"
  )

  set_target_properties(${TARGET_NAME} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

endfunction()