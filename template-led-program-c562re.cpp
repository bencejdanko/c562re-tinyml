// template-led-program.cpp

// OOP approach. `led` class encapsulates hardware details.
// `main()` calls led class.

// This is a baseline approach that needs improvements 
// For instance, the OOP class is dynamic, where
// port, moder, bindex are in RAM

#include <cstdint>

// we will describe mappings better
// in later examples 
#include "mcal_reg.h"

#include "mx_rcc.h"
// generated/hal/mx_rcc.c
// include the pre-generated configuration
// for setting the clock speeds
// allows for: 36 MHz, 48 MHz, 72 MHz, 144 MHz, 160 MHz
// modified so ENUM configurable in main()


// The Object-Oriented LED class
// microcontroller starts in a low power default state
// all peripherals are off
// gpio pins default to input/floating 
// safety, (so it doesn't send high voltage to
// external circuits by accident)
class led
{
public:
  using port_type = std::uint32_t;
  using bval_type = std::uint32_t;

  // Constructor: sets up clock, pin direction, and initial state
  led(const port_type p_odr,
      const port_type p_moder,
      const bval_type bit_index)
    : port(p_odr),
      moder(p_moder),
      bindex(bit_index)
  {
    // 1. Enable GPIOA clock in RCC
    *reinterpret_cast<volatile port_type*>(mcal::reg::rcc_ahb2enr)
      |= mcal::reg::rcc_ahb2enr_gpioaen;

    // 2. Set pin to Output mode ('01' in MODER)
    *reinterpret_cast<volatile port_type*>(moder)
      &= ~(0x3UL << (bindex * 2U));
    *reinterpret_cast<volatile port_type*>(moder)
      |=  (0x1UL << (bindex * 2U));

    // 3. Set pin initial state to LOW (OFF)
    // just convention
    // &= ~ (ON)
    // |= (OFF)
    *reinterpret_cast<volatile port_type*>(port)
      &= ~(1UL << bindex);
  }

  // Toggle method
  void toggle() const
  {
    *reinterpret_cast<volatile port_type*>(port)
      ^= (1UL << bindex);
  }

private:
  const port_type port;   // ODR address
  const port_type moder;  // MODER address
  const bval_type bindex; // Pin number (e.g. 5 for PA5)
};

// Anonymous namespace for file-local instances
namespace
{
  
  // Simple software delay
  // This will be replaced with 
  // hardware timer interrupts (SysTick / Timers)
  // and cooperative task schedulers

  // by default the cortex m-33 (c562re) runs 48MHz
  // can go up to 144MHz / 160 MHz
  // or down to 24 MHz, 16 MHz, 4 MHz, 32.768 kHz 
  // Power∝Capacitance×Voltage^2×Frequency
  // max  (144-160) for DSP/FFT
  // medium for sensor/usb
  // low for triggers, sleep modes, temp sensors

  // below cmd translates also into
  
  /*
  .loop:
    nop                 ; 1 cycle (Do nothing)
    adds  r3, r3, #1    ; 1 cycle (Increment i by 1)
    cmp   r3, r2        ; 1 cycle (Compare i to 400,000)
    bcc   .loop         ; 1 cycle (Branch back if less than)
  */

  // 3 clock cycles
  // 400000*3 -> 1,200,000
  // t = 1,200,000 / 48MHz = 0.025

  // for loops are also inaccurate and 
  // unpredictable -- so SysTick will be used later
  // ARM Cortex uses SysTick
  // Also hardware timers
  // DWT CYCCNT also counts exact # cycles

  void delay_loop()
  {
    for(std::uint32_t i = 0U; i < 400000UL; ++i)
    {
      asm volatile("nop");
    }
  }

  // Instantiate the LED object on PA5 (Green LED on NUCLEO-C562RE)
  const led led_a5
  {
    mcal::reg::gpioa_odr,
    mcal::reg::gpioa_moder,
    5U
  };
}

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
  mx_rcc_set_clock(CLOCK_PROFILE_750KHZ);

  // Forever-loop: toggle the LED object
  for(;;)
  {
    led_a5.toggle();
    delay_loop();
  }

  return 0;
}