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

// MCU memory map is located at stm32c562xe_flash.ld (line 28)
// Memory is between 0x200000000 and 0x20020000
TEST(TensorArenaSuite, VerifyStorageBufferPlacement)
{
  TensorArena<1024> tensor_arena{};
  std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(tensor_arena.data());

  EXPECT_GE(addr, 0x20000000UL);
  EXPECT_LE(addr + 1024U, 0x20020000UL);
  EXPECT_EQ(addr % 32U, 0U);
}

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
