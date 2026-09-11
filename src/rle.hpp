// rle.hpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

// Defines structures and algorithms for RLE compression and decompression.

#ifndef RLE_HPP_
#define RLE_HPP_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

// Fixed buffer for RLE input.
template<typename T, std::size_t MaxCapacity>
struct fixed_buffer
{
  alignas(16) std::array<T, MaxCapacity> data{};
  std::size_t size{0};

  bool push_back(const T& item)
  {
    if (size >= MaxCapacity) return false;
    data[size++] = item;
    return true;
  }

  void clear() { size = 0; }

  std::span<const T> view() const
  {
    return std::span<const T>(data.data(), size);
  }
};

// Basic struct for holding RLE representations.
struct rle_token
{
  std::uint8_t count;
  std::uint8_t value;

  constexpr bool operator==(const rle_token& other) const = default;
};

// Generates an incrementing step of numbers to the run length.
inline uint8_t step_generator(uint8_t run_length)
{
  static uint8_t current_val = 'A';
  static uint8_t count = 0;

  uint8_t out = current_val;
  count++;

  if (count >= run_length)
  {
    count = 0;
    current_val++;
    if (current_val > 'Z')
    {
      current_val = 'A';
    }
  }

  return out;
}

// RLE compressor algorithm.
template<std::size_t MaxTokens>
bool rle_compress(std::span<const std::uint8_t> input,
                  fixed_buffer<rle_token, MaxTokens>& output)
{
  output.clear();
  if (input.empty()) return true;

  std::size_t i = 0;
  while (i < input.size())
  {
    const std::uint8_t current_val = input[i];
    std::uint8_t count = 0;

    while (i < input.size() && input[i] == current_val && count < 255)
    {
      ++count;
      ++i;
    }

    if (!output.push_back(rle_token{count, current_val}))
    {
      return false;
    }
  }
  return true;
}

// RLE decompressor algorithm.
template<std::size_t MaxTokens>
bool rle_decompress(const fixed_buffer<rle_token, MaxTokens>& input,
                    std::span<std::uint8_t> output)
{
  if (input.size == 0) return true;

  std::size_t out_idx = 0;

  for (const auto& token : input.view())
  {
    if (out_idx + token.count > output.size())
    {
      return false;
    }

    std::fill_n(output.data() + out_idx, token.count, token.value);
    out_idx += token.count;
  }

  return true;
}

#endif  // RLE_HPP_
