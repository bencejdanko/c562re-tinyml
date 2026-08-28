// test_gpio_mcal.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

#include <cstdint>

#include "mcal_reg.h"
#include "pw_unit_test/framework.h"

// Verifies simulated peripheral register write and read-back value integrity
TEST(McalRegDynamicTest, SetAndGetRegisterValue)
{
  uint32_t simulated_reg = 0x00000000UL;
  const uintptr_t reg_addr = reinterpret_cast<uintptr_t>(&simulated_reg);

  mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::reg_set(reg_addr, 0x12345678UL);
  EXPECT_EQ(simulated_reg, 0x12345678UL);
  EXPECT_EQ((mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::reg_get(reg_addr)), 0x12345678UL);
}

// Tests atomic bit manipulation (set, bitwise NOT toggle, and clear)
TEST(McalRegDynamicTest, BitSetAndClearOperations)
{
  uint32_t simulated_reg = 0x00000000UL;
  const uintptr_t reg_addr = reinterpret_cast<uintptr_t>(&simulated_reg);

  // Set bit 5 (LED pin)
  mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::bit_set(reg_addr, 5U);
  EXPECT_EQ(simulated_reg, (1UL << 5U));

  // Toggle bit 5
  mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::bit_not(reg_addr, 5U);
  EXPECT_EQ(simulated_reg, 0x00000000UL);

  // Set bit 10
  mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::bit_set(reg_addr, 10U);
  EXPECT_EQ(simulated_reg, (1UL << 10U));

  // Clear bit 10
  mcal::reg::reg_access_dynamic<uintptr_t, uint32_t>::bit_clr(reg_addr, 10U);
  EXPECT_EQ(simulated_reg, 0x00000000UL);
}
