// encoding.cpp

// real time encoding RLE

// heap allocation is forbidden
// ex. new, malloc, std:vector:push_back
// non-deterministic allocation time & memory fragmentation

// use instead:
// std::array<T, N> // fixed stack storage
// std:span<T> // C++20 (pointer, size) pair
// fixed_buffer<T, Capacity> // static vector w/ `size` counter

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <array>
#include <span>

#include "mx_rcc.h"
#include "mx_usart2.h" // allows multiplexing output to UART
// #include "stm32c5xx_hal_rng.h" // hardware level random number generator


// retargets printf to COM port
extern "C" int __io_putchar(int ch) 
{
    HAL_UART_Transmit(mx_usart2_uart_gethandle(), reinterpret_cast<const uint8_t*>(&ch), 1, 0xFFFF);
    return ch;
}

// templates are compile-time structures
// fixed-capacity output
// can be used for any data type (typename)
// compile-time integer constant MaxCapacity
template<typename T, std::size_t MaxCapacity>
struct fixed_buffer
{
    std::array<T, MaxCapacity> data{};
    std::size_t size{0};

    // add a new element to the buffer
    bool push_back(const T& item)
    {
        if (size >= MaxCapacity) return false; // overflows
        data[size++] = item;
        return true;
    }

    void clear() { size = 0; }

    // C++20 feature
    // without span, if we pass say a `std::array<T, 64>`, there is no way to indicate
    // size, or where the rest is garbage 
    // and pointing to data.data() becomes dangerous

    // std::span is just 2 nums 
    // point to start of data (data.data()) and valid elements (size)

    /*
    Such that code becomes

    // range loop
    // auto& is readonly
    for (const auto& token : buffer.view())
    {
        // loops for `size` times
    }
    */
    // data just shorthand for &data[0], but can be ub is size is 0

    // compressed data is runtime read only (const)
    std::span<const T> view() const { return std::span<const T>(data.data(), size); }

    // we use this approach because we strictly want to bind our size
    // to our data, otherwise most memory and cve problems stem from this 

    /*
    For C++17:

    template <typename T>
    struct array_view
    {
        const T* data;
        std:size_t size;

        const T* begin()    const { return data; }
        const T* end()      const { return data + size; }
    };
    */
};

// class random_generator
// {
// public: 
//     random_generator()
//     {
//         HAL_RCC_RNG_EnableClock();
//         HAL_RNG_Init(&hrng, HAL_RNG);
//     }

//     std::uint32_t next()
//     {
//         std::uint_t random_val = 0;

//         HAL_RNG_GenerateRandomNumber(&hrng, &random_val);
//         return random_val;
//     }

// private:
//     hal_rng_handle_t hrng;
// }

class random_generator
{
public:
  explicit random_generator(std::uint32_t seed = 0x12345678UL) : state(seed) {}
  std::uint32_t next()
  {
    std::uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return state;
  }
  void fill_with_runs(std::span<std::uint8_t> buffer)
  {
    std::size_t i = 0;
    while (i < buffer.size())
    {
      const std::uint8_t symbol = static_cast<std::uint8_t>('A' + (next() % 6)); // 'A' through 'F'
      const std::size_t run_len = 1 + (next() % 8); // Run of 1 to 8 identical characters
      for (std::size_t r = 0; r < run_len && i < buffer.size(); ++r, ++i)
      {
        buffer[i] = symbol;
      }
    }
  }
private:
  std::uint32_t state;
};

// (Count, Value)
struct rle_token
{
    std::uint8_t count;
    std::uint8_t value;
};

// compressor algorithm
// takes in the random array, outputs to the fixed buffer
template<std::size_t MaxTokens>
bool rle_compress(std::span<const std::uint8_t> input, fixed_buffer<rle_token, MaxTokens>& output)
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


    // output.clear();
    // if (input.empty()) return true;

    // std::size_t i = 0;
    // while (i < input.size())
    // {
    //     const std::uint8_t current_val = input[i];
    //     std::uint8_t count = 0;

    //     while (i < input.size() && input[i] == current_val && count < 255)
    //     {
    //         ++count;
    //         ++i;
    //     }

    //     if (!output.push_back(rle_token{count, current_val}))
    //     {
    //         return false;
    //     }
    // }
    // return true;

int main()
{
  HAL_Init();
  mx_rcc_set_clock(CLOCK_PROFILE_144MHZ);
  mx_usart2_uart_init();

  std::array<std::uint8_t, 64> raw_data{};
  fixed_buffer<rle_token, 64> compressed_output{};
  random_generator rng(0xC0FFEE);

  std::uint32_t iteration = 1;

  for (;;)
  {
    rng.fill_with_runs(raw_data);

    for (std::uint8_t b : raw_data)
    {
      printf("%c", b);
    }
    printf("\"\r\n\r\n");

    if (rle_compress(raw_data, compressed_output))
    {
      const std::size_t compressed_bytes = compressed_output.size * sizeof(rle_token);
      const std::size_t ratio_pct = (compressed_bytes * 100) / raw_data.size();
      const std::size_t savings_pct = (ratio_pct <= 100) ? (100 - ratio_pct) : 0;


      for (const auto& token : compressed_output.view())
      {
        printf("[%u '%c'] ", token.count, token.value);
      }
      printf("\r\n\r\n");

      printf("  - Raw size:        %u bytes\r\n", static_cast<unsigned>(raw_data.size()));
      printf("  - Compressed size: %u bytes\r\n", static_cast<unsigned>(compressed_bytes));
      printf("  - Size Ratio:      %u%%\r\n", static_cast<unsigned>(ratio_pct));
      printf("  - Savings:         %u%%\r\n\r\n", static_cast<unsigned>(savings_pct));
    }
    else
    {
      printf("ERROR: Output buffer overflow\r\n");
    }

    for (std::uint32_t d = 0; d < 3000000UL; ++d)
    {
      asm volatile("nop");
    }
  }

  return 0;
}
