// test_tensor_arena.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko
//
// Unit test cases for a tensor arena.
// Verifies memory alignment.

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "pw_unit_test/framework.h"
#include "tensor_arena.hpp"

// Instantiated arena with default alignment (32)
// has a base address divisible by 32
TEST(TestTensorArena, AlignedBufferCreation)
{
  TensorArena<1024> arena{};
  // EXPECT_TRUE(reinterpret_cast<uintptr_t>(arena.data()) % 32 == 0);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(arena.data()) % 32U, 0U);
}

// Allocate two consecutive spans of uint8_t and verify their
// data pointers are aligned to 32 bytes, and do not overlap.
TEST(TestTensorArena, SequentialAllocation)
{
  TensorArena<1024> arena{};

  // Request sequentially 100 bytes and then 50 bytes.
  // This must align to 32 byte alignments (AXI optimization).
  auto tensor_a = arena.allocate<std::uint8_t>(100);
  auto tensor_b = arena.allocate<std::uint8_t>(50);
  ASSERT_TRUE(tensor_a.has_value());
  ASSERT_TRUE(tensor_b.has_value());

  // Expect size to match requested bytes.
  auto span_a = tensor_a.value();
  auto span_b = tensor_b.value();
  EXPECT_EQ(span_a.size(), 100U);
  EXPECT_EQ(span_b.size(), 50U);

  // verify alignment matches 32 bytes.
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(span_a.data()) % 32U, 0U);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(span_b.data()) % 32U, 0U);

  // The address of the 2nd allocated buffer must be greater
  // then that of span_a's plus it's size.
  EXPECT_GE(span_b.data(), span_a.data() + span_a.size());
}

// Request an allocation greater than CapacityBytes and verify
// a return of std::nullopt, and that used_bytes() is not advanced.
TEST(TestTensorArena, OutOfMemoryRejection)
{
  TensorArena<1024> arena{};

  auto tensor = arena.allocate<std::uint8_t>(2048);
  EXPECT_EQ(tensor, std::nullopt);

  EXPECT_EQ(arena.used_bytes(), 0U);
}

// Allocate buffer, record checkpoint(), allocate another buffer,
// rewind, and verify the subsequent allocations reuse the same
// memory space.
TEST(TestTensorArena, CheckpointAndRewind)
{
  TensorArena<1024> arena{};

  auto permanent = arena.allocate<std::uint8_t>(400);
  ASSERT_TRUE(permanent.has_value());

  std::size_t checkpoint = arena.checkpoint();
  EXPECT_EQ(checkpoint, arena.used_bytes());

  auto tensor = arena.allocate<std::uint8_t>(300);
  ASSERT_TRUE(tensor.has_value());
  void* scratch1_addr = tensor->data();

  EXPECT_TRUE(arena.rewind(checkpoint));
  EXPECT_EQ(checkpoint, arena.used_bytes());

  auto tensor2 = arena.allocate<std::uint8_t>(300);
  ASSERT_TRUE(tensor2.has_value());
  void* scratch2_addr = tensor2->data();

  EXPECT_EQ(scratch2_addr, scratch1_addr);
}
