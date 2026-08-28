// mcal_reg.h
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

// Microcontroller abstraction layer (MCAL) register access templates
// and register address definitions for STM32C562.

#ifndef MCAL_REG_H_
#define MCAL_REG_H_

#include <cstdint>

namespace mcal
{
namespace reg
{
// Dynamic register access helper
template<typename RegisterAddressType, typename RegisterValueType>
struct reg_access_dynamic final
{
  static void reg_set(const RegisterAddressType address, const RegisterValueType val)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) = val;
  }

  static auto reg_get(const RegisterAddressType address) -> RegisterValueType
  {
    return *reinterpret_cast<volatile RegisterValueType*>(address);
  }

  static void reg_or(const RegisterAddressType address, const RegisterValueType val)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) |= val;
  }

  static void reg_and(const RegisterAddressType address, const RegisterValueType val)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) &= val;
  }

  static void bit_set(const RegisterAddressType address, const RegisterValueType bit_pos)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) |=
        static_cast<RegisterValueType>(1ULL << bit_pos);
  }

  static void bit_clr(const RegisterAddressType address, const RegisterValueType bit_pos)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) &=
        static_cast<RegisterValueType>(~(1ULL << bit_pos));
  }

  static void bit_not(const RegisterAddressType address, const RegisterValueType bit_pos)
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) ^=
        static_cast<RegisterValueType>(1ULL << bit_pos);
  }
};

// Static (compile-time address & value) register access helper
template<typename RegisterAddressType, typename RegisterValueType,
         const RegisterAddressType address,
         const RegisterValueType value = static_cast<RegisterValueType>(0)>
struct reg_access_static final
{
  static void reg_set() { *reinterpret_cast<volatile RegisterValueType*>(address) = value; }

  static void reg_or() { *reinterpret_cast<volatile RegisterValueType*>(address) |= value; }

  static void reg_and() { *reinterpret_cast<volatile RegisterValueType*>(address) &= value; }

  static auto reg_get() -> RegisterValueType
  {
    return *reinterpret_cast<volatile RegisterValueType*>(address);
  }

  static void bit_set()
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) |=
        static_cast<RegisterValueType>(1ULL << value);
  }

  static void bit_clr()
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) &=
        static_cast<RegisterValueType>(~(1ULL << value));
  }

  static void bit_not()
  {
    *reinterpret_cast<volatile RegisterValueType*>(address) ^=
        static_cast<RegisterValueType>(1ULL << value);
  }
};

// STM32C562 Register Definitions for Chapter 1 LED example
// AHB2 Base: 0x42020000UL, GPIOA Base: 0x42020000UL
constexpr std::uint32_t gpioa_moder = 0x42020000UL + 0x00UL;
constexpr std::uint32_t gpioa_odr = 0x42020000UL + 0x14UL;
constexpr std::uint32_t gpioa_bsrr = 0x42020000UL + 0x18UL;

// AHB3 Base: 0x44020000UL, RCC Base: 0x44020C00UL
constexpr std::uint32_t rcc_ahb2enr = 0x44020C00UL + 0x8CUL;
constexpr std::uint32_t rcc_ahb2enr_gpioaen = 0x01UL;
}  // namespace reg
}  // namespace mcal

#endif  // MCAL_REG_H_
