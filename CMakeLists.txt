# file-format: 1.0.0
cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

include(cmake/target.cmake)
include(cmake/${TOOLCHAIN_FILE})

project(c562re)
add_executable(c562re)
include(cmake/files.cmake)
include(cmake/flags.cmake)
include(cmake/components.cmake)

# Stubs to be able to add compiler flag overrides, e.g. -Os
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  # Add additional source files here
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  src
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional defined symbols here
)

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional compiler flags here
)

target_link_options(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker flags here
)

target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker search directories here
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker libraries here
)

# --- Native Developer Targets (Size & Flash) ---
if(CMAKE_SIZE)
  add_custom_target(size
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Firmware Memory Section Sizes:"
  )
endif()

find_program(OPENOCD_BIN openocd)
find_program(STM32_PROG_CLI STM32_Programmer_CLI
  PATHS "$ENV{HOME}/.local/share/stm32cube/bundles/programmer/2.23.0/bin"
)

if(STM32_PROG_CLI)

  add_custom_target(flash
    COMMAND ${STM32_PROG_CLI} -c port=SWD -w $<TARGET_FILE:${CMAKE_PROJECT_NAME}> -v -rst
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Flashing firmware over SWD via STM32_Programmer_CLI..."
  )
elseif(OPENOCD_BIN)
  add_custom_target(flash
    COMMAND ${OPENOCD_BIN} -f ${CMAKE_SOURCE_DIR}/openocd.cfg -c "program $<TARGET_FILE:${CMAKE_PROJECT_NAME}> verify reset exit"
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Flashing firmware over SWD via OpenOCD..."
  )
endif()

# Native CMake OpenOCD Server target
if(OPENOCD_BIN)
  add_custom_target(openocd
    COMMAND ${OPENOCD_BIN} -f ${CMAKE_SOURCE_DIR}/openocd.cfg
    USES_TERMINAL
    COMMENT "Starting OpenOCD GDB Server on localhost:3333..."
  )
endif()

# Native CMake Debug target
find_program(GDB_BIN NAMES gdb-multiarch arm-none-eabi-gdb gdb)
if(GDB_BIN)
  add_custom_target(debug
    COMMAND ${GDB_BIN} -q -ex "connect" $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    DEPENDS ${CMAKE_PROJECT_NAME}
    USES_TERMINAL
    COMMENT "Launching GDB Debugger..."
  )
endif()

# Native CMake Format & Check-Format targets
find_program(CLANG_FORMAT_BIN clang-format)
if(CLANG_FORMAT_BIN)
  file(GLOB_RECURSE PROJECT_FORMAT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_SOURCE_DIR}/tests/*.hpp
    ${CMAKE_SOURCE_DIR}/tests/*.c
    ${CMAKE_SOURCE_DIR}/tests/*.h
  )
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_BIN} -i ${PROJECT_FORMAT_SOURCES}
    COMMENT "Formatting src/ and tests/ with clang-format..."
  )
  add_custom_target(check-format
    COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror ${PROJECT_FORMAT_SOURCES}
    COMMENT "Checking clang-format compliance across src/ and tests/..."
  )
endif()



