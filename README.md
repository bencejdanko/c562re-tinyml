# STM32C562RE TinyML

```
[x] 32-bit aligned Tensor Arena 
[ ] RAII DWT / PMU Profiler
[ ] Ping Pong Buffer 
[ ] INT8 Quantized Kernel and Parity Engine
[ ] Cache and Memory Manager
[ ] FOMO Deployment
[ ] Pigweed Benchmarking and Tracing
[ ] Nordic PPK2 and LA2016 power and latency validation
```

## Programs

```
tensor_arena.hpp            # 32-bit alignment tensor arena implementation
```

## Configuration

`stm32c562xe_flash.ld` describes the physical datasheet for the linker.

[Datasheet](https://www.st.com/en/microcontrollers-microprocessors/stm32c552re.html)

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

# Check formatting compliance without modifying files
cmake --build --preset debug_GCC_NUCLEO-C562RE --target check-format
```

## Testing Procedure

### Target Hardware Testing (STM32C562RE)

```
cmake --preset target_test_NUCLEO-C562RE
cmake --build --preset target_test_NUCLEO-C562RE --target flash
uv run pytest -s
```

### Host Testing

```
cmake -B build/host_test -S tests -G Ninja
cmake --build build/host_test
ctest --test-dir build/host_test --output-on-failure
```
