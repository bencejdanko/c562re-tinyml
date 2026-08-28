# Real Time and Efficient C++ on the STM32C562RE

## Programs

```
src/c562re-template-led-program.cpp     # LED example
src/c562re-rle-encoding-decoding.cpp    # RLE compression example
```

## Configuration

Set flags in `cmake/flags.cmake`
Specify build target file w/ `cmake/files.cmake`.
Modify heap and stack allocations in `user_modifiable/Device/STM32C562RET6/stm32c562xe_flash.ld`

## Requirements

- cmake
- ninja
- arm-none-eabi-gcc
- arm-none-eabi-g++
- openocd
- gdb-multiarch
- arm-none-eabi-gdb
- clang-format

Testing also depends on the Pigweed framework. Use the submodule to get the exact version.

## Build and Flash

Select which file to build in `cmake/files.cmake`.

```bash
# Build with
cmake --preset debug_GCC_NUCLEO-C562RE
cmake --build --preset debug_GCC_NUCLEO-C562RE

# Check memory sizes
cmake --build --preset debug_GCC_NUCLEO-C562RE --target size

# Flash to the board
cmake --build --preset debug_GCC_NUCLEO-C562RE --target flash

# Start OpenOCD GDB server
cmake --build --preset debug_GCC_NUCLEO-C562RE --target openocd

# Launch GDB
cmake --build --preset debug_GCC_NUCLEO-C562RE --target debug

# Auto-format source files
cmake --build --preset debug_GCC_NUCLEO-C562RE --target format
```

## Host Unit Testing

```bash
# Configure and run unit tests on host PC
cmake -B build/host_test -S tests -G Ninja
cmake --build build/host_test
ctest --test-dir build/host_test --output-on-failure
```

Also configured as Github Action jobs.

All unit tests located in `tests/unit/*`.

## MCU Testing

I/O has been configured with `src/pw_sys_io_stm32.cpp`. Correctness of tests relayed through UART.

All hardware-level tests located in `tests/target/*`

Use the below script to orchestrate a hardware testing run (requires the C562RE to be connected):

```bash
python3 tests/target/run_target_tests.py
```
