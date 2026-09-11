// c562re-rle-encoding-decoding.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

#include <cstdio>

#include "mx_rcc.h"
#include "rle.hpp"

class periodic_timer
{
public:
  explicit periodic_timer(uint32_t period_ms)
      : period(period_ms), last_time(HAL_GetTick())
  {
  }

  bool timeout()
  {
    uint32_t now = HAL_GetTick();

    if (static_cast<uint32_t>(now - last_time) >= period)
    {
      last_time += period;
      return true;
    }

    return false;
  }

private:
  uint32_t period;
  uint32_t last_time;
};

int main()
{
  HAL_Init();
  mx_rcc_set_clock(CLOCK_PROFILE_48MHZ);

  // RLE encoding and decoding buffers.
  std::array<std::uint8_t, 64> raw_data{};
  fixed_buffer<rle_token, 64> compressed_output{};
  std::array<std::uint8_t, 64> uncompressed_raw_data{};

  periodic_timer rle_task_timer{100};  // ms

  for (;;)
  {
    if (rle_task_timer.timeout())
    {
      // Fill the raw_data buffer with
      // stepped character sequences (up to 8 repetitions)
      for (auto& b : raw_data)
      {
        b = step_generator(8);
      }

      if (!rle_compress(raw_data, compressed_output))
      {
        return 1;
      }

      if (!rle_decompress(compressed_output, uncompressed_raw_data))
      {
        return 1;
      }
    }
  }

  return 0;
}
