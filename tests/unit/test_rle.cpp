// test_rle.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

#include <array>
#include <cstdint>

#include "pw_unit_test/framework.h"
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

// Verifies clear resets the buffer logical size back to zero
TEST(FixedBufferTest, ClearResetsSize)
{
  fixed_buffer<uint8_t, 4> buffer{};
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.clear();
  EXPECT_EQ(buffer.size, 0U);
  EXPECT_TRUE(buffer.view().empty());
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
  ASSERT_EQ(output.size, 1U);
  EXPECT_EQ(output.data[0].count, 5U);
  EXPECT_EQ(output.data[0].value, 'A');
}

// Tests multi-character alternating sequence tokenization
TEST(RleCodecTest, CompressesAlternatingPattern)
{
  std::array<uint8_t, 6> input{'A', 'A', 'B', 'B', 'B', 'C'};
  fixed_buffer<rle_token, 4> output{};

  EXPECT_TRUE(rle_compress(input, output));
  ASSERT_EQ(output.size, 3U);

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
  ASSERT_TRUE(rle_compress(original, compressed));

  std::array<uint8_t, 32> recovered{};
  ASSERT_TRUE(rle_decompress(compressed, recovered));

  EXPECT_EQ(original, recovered);
}
