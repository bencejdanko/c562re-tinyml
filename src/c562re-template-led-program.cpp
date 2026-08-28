// c562re-template-led-program.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

// Controls a GPIO driving an LED to blink.

#include <cstdint>

#include "mcal_reg.h"
#include "mx_rcc.h"

// Controls a GPIO pin driving an LED.
//
// Encapsulates the runtime register addresses and bit indexes for a pin.
// Configures the pin as general-purpose output on construction.
class Led
{
public:
  using port_type = std::uint32_t;
  using bval_type = std::uint32_t;

  Led(const port_type p_odr, const port_type p_moder, const bval_type bit_index)
      : port(p_odr), moder(p_moder), bindex(bit_index)
  {
    // Enable GPIOA clock in RCC
    *reinterpret_cast<volatile port_type*>(mcal::reg::rcc_ahb2enr) |=
        mcal::reg::rcc_ahb2enr_gpioaen;

    // Set pin to Output mode ('01' in MODER)
    *reinterpret_cast<volatile port_type*>(moder) &= ~(0x3UL << (bindex * 2U));
    *reinterpret_cast<volatile port_type*>(moder) |= (0x1UL << (bindex * 2U));

    // Set pin initial state to LOW (OFF)
    *reinterpret_cast<volatile port_type*>(port) |= ~(1UL << bindex);
  }

  void toggle() const { *reinterpret_cast<volatile port_type*>(port) ^= (1UL << bindex); }

private:
  const port_type port;    // ODR address
  const port_type moder;   // MODER address
  const bval_type bindex;  // Pin number
};

namespace
{

// Shimmy for loop for pause
void delay_loop()
{
  for (std::uint32_t i = 0U; i < 400000UL; ++i)
  {
    asm volatile("nop");
  }
}

const Led led_a5{mcal::reg::gpioa_odr, mcal::reg::gpioa_moder, 5U};
}  // namespace

int main()
{
  //   mx_rcc_set_clock(CLOCK_PROFILE_160MHZ); // 160 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_144MHZ); // 144 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_100MHZ); // 100 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_48MHZ);  //  48 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_24MHZ);  //  24 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_12MHZ);  //  12 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_6MHZ);   //   6 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_3MHZ);   //   3 MHz
  //   mx_rcc_set_clock(CLOCK_PROFILE_750KHZ); // 750 kHz
  mx_rcc_set_clock(CLOCK_PROFILE_48MHZ);

  for (;;)
  {
    led_a5.toggle();
    delay_loop();
  }

  return 0;
}
