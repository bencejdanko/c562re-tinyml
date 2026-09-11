// target_test_main.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

// Monofile for hardware-level tests for the microcontroller.

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "mx_system.h"
#include "pw_sys_io/sys_io.h"
#include "pw_unit_test/framework.h"
#include "pw_unit_test/simple_printing_event_handler.h"
#include "tensor_arena.hpp"

#include "mx_rcc.h"


// sbbs: represents the beginning of BSS memory
// ebss: represents the end of it
// __StackLimit: The most the stack should extend before heap begins
// __StackTop: The beginning of the stack writes
extern "C" std::uint8_t _sbss[], _ebss[], __StackLimit[], __StackTop[];

// Define TensorArena in .bss memory.
// Ensure it exists within it's limits
static TensorArena<1024> in_bss_tensor_arena{};
TEST(TensorArenaSuite, ArenaLivesInBss)
{
  const auto addr = reinterpret_cast<std::uintptr_t>(in_bss_tensor_arena.data());
  const auto bss_start = reinterpret_cast<std::uintptr_t>(_sbss);
  const auto bss_end = reinterpret_cast<std::uintptr_t>(_ebss);

  EXPECT_GE(addr, bss_start);
  EXPECT_LE(addr + sizeof(in_bss_tensor_arena), bss_end);

  // expect 32-bit alignment
  EXPECT_EQ(addr % 32U, 0U);
}

// Verify stack memory placement
TEST(TensorArenaSuite, VerifyStorageBufferPlacement)
{
  TensorArena<1024> tensor_arena{};
  std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(tensor_arena.data());
  const auto stack_start = reinterpret_cast<std::uintptr_t>(__StackLimit);
  const auto stack_end = reinterpret_cast<std::uintptr_t>(__StackTop);

  EXPECT_GE(addr, stack_start);
  EXPECT_LE(addr + sizeof(tensor_arena), stack_end);
  
  // expect 32-bit alignment
  EXPECT_EQ(addr % 32U, 0U);
}

// Verify clock reconfiguring works
// By default, the system starts at 144MHz

TEST(ClockSuite, VerifyClockRetiming)
{
  mx_rcc_set_clock(CLOCK_PROFILE_144MHZ); // Should 
  EXPECT_EQ((HAL_RCC_GetSYSCLKFreq()), 144'000'000U);
}

// Bit integrity and execution timing

static void WriteStringToSysIo(std::string_view s, bool append_newline)
{
  if (append_newline)
  {
    pw::sys_io::WriteLine(s);
  }
  else
  {
    for (char c : s)
    {
      pw::sys_io::WriteByte(static_cast<std::byte>(c));
    }
  }
}

int main()
{
  mx_system_init();

  pw::unit_test::SimplePrintingEventHandler handler(WriteStringToSysIo, false);
  pw::unit_test::RegisterEventHandler(&handler);

  for (;;)
  {
    pw::sys_io::WriteLine("\r\n[==========] Google Pigweed (pw_unit_test:light) Test Runner");
    pw::sys_io::WriteLine("[  TARGET  ] NUCLEO-C562RE (STM32C562RET6 ARM Cortex-M33 @ 120 MHz)");

    RUN_ALL_TESTS();

    HAL_Delay(3000);  // Continuous telemetry repeat interval
  }

  return 0;
}
