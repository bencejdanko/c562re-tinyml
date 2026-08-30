# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(c562re_tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wshadow
  -fsanitize=address,undefined
  -fno-omit-frame-pointer
  -g
)
add_link_options(
  -fsanitize=address,undefined
)

enable_testing()

set(PW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/pigweed)

add_library(pw_unit_test_host STATIC
  ${PW_DIR}/pw_unit_test/framework_light.cc
  ${PW_DIR}/pw_unit_test/googletest_style_event_handler.cc
  ${PW_DIR}/pw_unit_test/simple_printing_event_handler.cc
  ${PW_DIR}/pw_unit_test/simple_printing_main.cc
  ${PW_DIR}/pw_assert_basic/assert_basic.cc
  ${PW_DIR}/pw_assert_basic/basic_handler.cc
  ${PW_DIR}/pw_string/string_builder.cc
  ${PW_DIR}/pw_string/type_to_string.cc
  ${PW_DIR}/pw_string/format.cc
  ${PW_DIR}/pw_sys_io/sys_io.cc
  ${PW_DIR}/pw_sys_io_stdio/sys_io.cc
)

target_include_directories(pw_unit_test_host PUBLIC
  ${PW_DIR}/pw_unit_test/public
  ${PW_DIR}/pw_unit_test/light_public_overrides
  ${PW_DIR}/pw_unit_test/public_overrides
  ${PW_DIR}/pw_preprocessor/public
  ${PW_DIR}/pw_polyfill/public
  ${PW_DIR}/pw_status/public
  ${PW_DIR}/pw_result/public
  ${PW_DIR}/pw_span/public
  ${PW_DIR}/pw_bytes/public
  ${PW_DIR}/pw_string/public
  ${PW_DIR}/pw_assert/public
  ${PW_DIR}/pw_assert/public_overrides
  ${PW_DIR}/pw_assert/print_and_abort_assert_public_overrides
  ${PW_DIR}/pw_assert_basic/public
  ${PW_DIR}/pw_assert_basic/public_overrides
  ${PW_DIR}/pw_sys_io/public
  ${PW_DIR}/pw_containers/public
  ${PW_DIR}/pw_numeric/public
  ${PW_DIR}/third_party/fuchsia/repo/sdk/lib/stdcompat/include
)

# Auto-discover unit tests via a test_ prefix.
file(GLOB UNIT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/unit/test_*.cpp")

foreach(TEST_SRC ${UNIT_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

  add_executable(${TEST_NAME} ${TEST_SRC})
  target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
  target_link_libraries(${TEST_NAME} PRIVATE pw_unit_test_host)
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

find_program(CLANG_FORMAT_BIN clang-format)
if(CLANG_FORMAT_BIN)
  file(GLOB_RECURSE TEST_FORMAT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
  )
  add_test(
    NAME clang_format_check
    COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror ${TEST_FORMAT_SOURCES}
  )
endif()



