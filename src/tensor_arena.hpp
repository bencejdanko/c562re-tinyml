// tensor_arena.hpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko
//
// Tensor arena implementation with 32 byte alignment.

#ifndef TENSOR_ARENA_HPP_
#define TENSOR_ARENA_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>

// fixed tensor arena. Takes size and alignment, which by default is 32 bytes.
template<std::size_t CapacityBytes, std::size_t Alignment = 32>
struct TensorArena
{
  // The arena just contains raw bytes, for any kind of workload.
  // Value initialization so bytes are 0.
  alignas(Alignment) std::array<std::byte, CapacityBytes> storage_{};

  // Ensures value initialization,
  // so offset starts at 0.
  std::size_t offset_{0};

  // Return the start of memory.
  // Ensure that it cannot be discarded to prevent memory leaks.
  // No exception overhead.
  [[nodiscard]] const std::byte* data() const noexcept
  {
    return storage_.data();
  }

  // function that returns the current offset_
  [[nodiscard]] std::size_t used_bytes() const noexcept { return offset_; }

  // function to return capacity bytes
  [[nodiscard]] constexpr std::size_t capacity() const noexcept
  {
    return CapacityBytes;
  }

  // functioning to return the remaining bytes
  [[nodiscard]] std::size_t remaining_bytes() const noexcept
  {
    return CapacityBytes - offset_;
  }

  // set offset to 0
  constexpr void reset() noexcept { offset_ = 0U; }

  // return the offset
  [[nodiscard]] std::size_t checkpoint() const noexcept { return offset_; }

  // rewind
  // Checks if the cp <= offset, then sets offset to it, then true/false
  [[nodiscard]] bool rewind(std::size_t cp) noexcept
  {
    if (cp <= offset_)
    {
      offset_ = cp;
      return true;
    }

    return false;
  }

  // Allocation to allocate within the buffer.
  template<typename T>
  [[nodiscard]] std::optional<std::span<T>> allocate(std::size_t count) noexcept
  {
    // Calculate how much bytes type T with count elements needs.
    std::size_t byte_count = count * sizeof(T);

    // find the current raw pointer location within the storage_.
    std::uintptr_t current_address =
        reinterpret_cast<std::uintptr_t>(storage_.data() + offset_);

    // align the current address up to required alignment.
    // here we apply the bitmask alignment formula.
    std::uintptr_t aligned_address =
        (current_address + Alignment - 1U) & ~(Alignment - 1U);

    // calculate the amount of bytes advanced to use for alignment padding
    std::size_t padding = aligned_address - current_address;

    // check if this will exceed memory.
    // if so, return an std::nullopt.
    if (offset_ + padding + byte_count > CapacityBytes)
    {
      return std::nullopt;
    }

    // advance the offset
    offset_ += (padding + byte_count);

    // cast aligned address and return span view
    T* typed_ptr = reinterpret_cast<T*>(aligned_address);
    return std::span<T>(typed_ptr, count);
  }
};
#endif  // TENSOR_ARENA_HPP_
