# Real Time and Efficient C++ on the STM32C562RE

The original template code is available at https://github.com/bencejdanko/nucleo-c562re-workshop. It features a blinking LED, saving ADC data to EEPROM via button, UART communication

This guide hopes to demonstrate

- Template-based register access
- Static polymorphism & zero-cost abstractions
- Object-oriented peripheral drivers (GPIO, UART, Timers)
- Custom fixed-size containers & memory pools
- Real-time multitasking & scheduler

## CMake

In `cmake/flags.cmake`, set to compile with 

```bash
arm-none-eabi-g++ \         # cross compiler -- arm target, no os (bare metal),
                            # embedded application binary enterface, 
                            # G++ compiler front end
    -std=c++20 \            # C++20 Standard
    -fno-rtti \             # Disable Run Time Type Information
                            # Disables Metadata tables
    -fno-exceptions \       # exceptions bloat the stack, 
                            # have undeterministic timing,
                            # dynamic memory overflows
                            # better to use std::abort() or handler 
    -ffunction-sections \   # 
    -fdata-sections         # functions placed in isolated section
    Wl,--gc-sections        # garbage collect unused sections
                            # drivers not called by main .cpp is
                            # automatically discarded by the linker
```

In `cmake/files.cmake` you can specify which project to compile (e.g. main.cpp)

## Approaches

ST32Micro uses C HAL approach, so there are heavy C libraries to interface with say, `HAL_GPIO_WritePin` or `HAL_UART_Transmit`. We also want to introduce type safe OOP abstractions over hardware registers w/ templates, constexpr

We explore both

## Current programs

```
template-led-program-c562re.cpp     # LED example
```

## Ubuntu

Compile the binary: 

```
cmake --preset debug_GCC_NUCLEO-C562RE
cmake --build --preset debug_GCC_NUCLEO-C562RE
```

Flash to the board:

```bash
# this creates workshop.elf -- Executable and Linkable Format
# .text -- machine code
# .rodata, .data -- constants and data
# Memory map headers
# debug symbols for setting breakpoints
# Rename the genned executable in CMakeLists.txt

# We connect via SWD (Serial Wire Debug), w wire hardware interface
# via onboard STLink programmer
# -w erases flash mem and writes to internal flash mem 
# -v verifies for non-corruption
# -rst simple reset so it reboots w/ new program

~/.local/share/stm32cube/bundles/programmer/2.23.0/bin/STM32_Programmer_CLI \
    -c port=SWD \
    -w build/debug_GCC_NUCLEO-C562RE/c562re.elf \
    -v \
    -rst
```


