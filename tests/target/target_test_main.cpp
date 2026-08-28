// target_test_main.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

#include "mcal_reg.h"
#include "mx_rcc.h"
#include "mx_system.h"
#include "mx_usart2.h"
#include "pw_sys_io/sys_io.h"
#include "pw_unit_test/framework.h"
#include "pw_unit_test/simple_printing_event_handler.h"
#include "rle.hpp"

// Verifies that a zero-initialized buffer reports size == 0 and an empty span view
TEST(FixedBufferTest, InitializesEmpty)
{
  fixed_buffer<uint8_t, 8> buffer{};
  EXPECT_EQ(buffer.size, 0U);
  EXPECT_TRUE(buffer.view().empty());
}

// Confirms elements are appended sequentially up to static capacity
TEST(FixedBufferTest, PushBackWithinCapacity)
{
  fixed_buffer<uint8_t, 4> buffer{};
  EXPECT_TRUE(buffer.push_back(10));
  EXPECT_TRUE(buffer.push_back(20));
  EXPECT_EQ(buffer.size, 2U);
  EXPECT_EQ(buffer.data[0], 10);
  EXPECT_EQ(buffer.data[1], 20);
}

// Confirms that attempting to push past capacity returns false without memory corruption
TEST(FixedBufferTest, PushBackOverflowRejection)
{
  fixed_buffer<uint8_t, 2> buffer{};
  EXPECT_TRUE(buffer.push_back(1));
  EXPECT_TRUE(buffer.push_back(2));
  EXPECT_FALSE(buffer.push_back(3));
  EXPECT_EQ(buffer.size, 2U);
}

// Verifies that buffer storage is aligned to a 16-byte boundary for Cortex-M33 DMA and I-Cache
TEST(FixedBufferTest, MemoryAlignment16Byte)
{
  fixed_buffer<rle_token, 64> buffer{};
  uintptr_t addr = reinterpret_cast<uintptr_t>(buffer.data.data());
  EXPECT_EQ((addr % 16), 0U);
}

// Handles zero-length input edge cases gracefully
TEST(RleCodecTest, CompressesEmptyInput)
{
  std::array<uint8_t, 0> input{};
  fixed_buffer<rle_token, 4> output{};
  EXPECT_TRUE(rle_compress(input, output));
  EXPECT_EQ(output.size, 0U);
}

// Verifies run-length counting for homogeneous character runs
TEST(RleCodecTest, CompressesSingleCharacterRun)
{
  std::array<uint8_t, 5> input{'A', 'A', 'A', 'A', 'A'};
  fixed_buffer<rle_token, 4> output{};

  EXPECT_TRUE(rle_compress(input, output));
  EXPECT_EQ(output.size, 1U);
  EXPECT_EQ(output.data[0].count, 5U);
  EXPECT_EQ(output.data[0].value, 'A');
}

// Tests multi-character alternating sequence tokenization
TEST(RleCodecTest, CompressesAlternatingPattern)
{
  std::array<uint8_t, 6> input{'A', 'A', 'B', 'B', 'B', 'C'};
  fixed_buffer<rle_token, 4> output{};

  EXPECT_TRUE(rle_compress(input, output));
  EXPECT_EQ(output.size, 3U);
  EXPECT_EQ(output.data[0].count, 2U);
  EXPECT_EQ(output.data[0].value, 'A');
  EXPECT_EQ(output.data[1].count, 3U);
  EXPECT_EQ(output.data[1].value, 'B');
  EXPECT_EQ(output.data[2].count, 1U);
  EXPECT_EQ(output.data[2].value, 'C');
}

// Guards against output buffer overflow when compressed token count exceeds capacity
TEST(RleCodecTest, CompressionFailsOnOutputOverflow)
{
  std::array<uint8_t, 6> input{'A', 'B', 'C', 'D', 'E', 'F'};
  fixed_buffer<rle_token, 2> output{};
  EXPECT_FALSE(rle_compress(input, output));
}

// Bit-exact round-trip compression and decompression verification across pseudo-random pattern
TEST(RleCodecTest, RoundTripCompressDecompressBitExact)
{
  std::array<uint8_t, 32> original{};
  for (std::size_t i = 0; i < original.size(); ++i)
  {
    original[i] = step_generator(4);
  }

  fixed_buffer<rle_token, 32> compressed{};
  EXPECT_TRUE(rle_compress(original, compressed));

  std::array<uint8_t, 32> recovered{};
  EXPECT_TRUE(rle_decompress(compressed, recovered));

  for (std::size_t i = 0; i < 32; ++i)
  {
    EXPECT_EQ(original[i], recovered[i]);
  }
}

// Verifies monotonic ARM Cortex-M SysTick timer tick advancement
TEST(HardwareMcalTest, SysTickMonotonicAdvance)
{
  uint32_t t1 = HAL_GetTick();
  HAL_Delay(10);
  uint32_t t2 = HAL_GetTick();
  EXPECT_TRUE(t2 >= t1 + 10);
}

// Performs atomic bit set, clear, and verification on physical GPIOA_ODR register
TEST(HardwareMcalTest, GpioRegisterBitManipulation)
{
  uint32_t orig_odr =
      mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::reg_get(mcal::reg::gpioa_odr);

  mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::bit_set(mcal::reg::gpioa_odr, 5U);
  uint32_t set_odr =
      mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::reg_get(mcal::reg::gpioa_odr);
  EXPECT_TRUE((set_odr & (1UL << 5U)) != 0);

  mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::bit_clr(mcal::reg::gpioa_odr, 5U);
  uint32_t clr_odr =
      mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::reg_get(mcal::reg::gpioa_odr);
  EXPECT_TRUE((clr_odr & (1UL << 5U)) == 0);

  mcal::reg::reg_access_dynamic<uint32_t, uint32_t>::reg_set(mcal::reg::gpioa_odr, orig_odr);
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
    pw::sys_io::WriteLine("[  SUITES  ] FixedBufferTest | RleCodecTest | HardwareMcalTest");

    RUN_ALL_TESTS();

    HAL_Delay(3000);  // Continuous telemetry repeat interval
  }

  return 0;
}
