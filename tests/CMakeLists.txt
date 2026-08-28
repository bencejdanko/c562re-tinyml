# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(c562re_tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Pigweed include directories and library for host unit testing
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

# Test executable for RLE Codec and Buffer unit tests
add_executable(test_rle
  unit/test_rle.cpp
)
target_include_directories(test_rle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(test_rle PRIVATE pw_unit_test_host)
add_test(NAME test_rle COMMAND test_rle)

# Test executable for MCAL Register Mock tests
add_executable(test_gpio_mcal
  unit/test_gpio_mcal.cpp
)
target_include_directories(test_gpio_mcal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(test_gpio_mcal PRIVATE pw_unit_test_host)
add_test(NAME test_gpio_mcal COMMAND test_gpio_mcal)

