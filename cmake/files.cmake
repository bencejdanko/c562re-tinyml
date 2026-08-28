if(CMAKE_BUILD_TYPE STREQUAL "debug_GCC_NUCLEO-C562RE")
  if(BUILD_TARGET_TESTS)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
      tests/target/target_test_main.cpp
      src/pw_sys_io_stm32.cpp
      third_party/pigweed/pw_unit_test/framework_light.cc
      third_party/pigweed/pw_unit_test/googletest_style_event_handler.cc
      third_party/pigweed/pw_unit_test/simple_printing_event_handler.cc
      third_party/pigweed/pw_assert_basic/assert_basic.cc
      third_party/pigweed/pw_assert_basic/basic_handler.cc
      third_party/pigweed/pw_string/string_builder.cc
      third_party/pigweed/pw_string/type_to_string.cc
      third_party/pigweed/pw_string/format.cc
    )

    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_unit_test/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_unit_test/light_public_overrides
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_unit_test/public_overrides
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_preprocessor/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_polyfill/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_status/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_result/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_span/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_bytes/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_string/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_assert/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_assert/public_overrides
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_assert/print_and_abort_assert_public_overrides
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_assert_basic/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_assert_basic/public_overrides
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_sys_io/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_containers/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/pw_numeric/public
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pigweed/third_party/fuchsia/repo/sdk/lib/stdcompat/include
    )
  else()
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/c562re-rle-encoding-decoding.cpp)
  endif()
endif()

