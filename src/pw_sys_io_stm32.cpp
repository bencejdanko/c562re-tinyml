// pw_sys_io_stm32.cpp
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Bence Danko

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "mx_usart2.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_sys_io/sys_io.h"
#include "stm32_hal.h"

namespace pw::sys_io
{

Status ReadByte(std::byte* dest)
{
  uint8_t c = 0;
  if (HAL_UART_Receive(mx_usart2_uart_gethandle(), &c, 1, HAL_MAX_DELAY) !=
      HAL_OK)
  {
    return Status::ResourceExhausted();
  }
  *dest = static_cast<std::byte>(c);
  return OkStatus();
}

Status TryReadByte(std::byte* /*dest*/) { return Status::Unimplemented(); }

Status WriteByte(std::byte b)
{
  uint8_t c = static_cast<uint8_t>(b);
  if (HAL_UART_Transmit(mx_usart2_uart_gethandle(), &c, 1, 1000) != HAL_OK)
  {
    return Status::ResourceExhausted();
  }
  return OkStatus();
}

StatusWithSize WriteLine(std::string_view s)
{
  for (char ch : s)
  {
    if (!WriteByte(static_cast<std::byte>(ch)).ok())
    {
      return StatusWithSize(Status::ResourceExhausted(), 0);
    }
  }
  WriteByte(static_cast<std::byte>('\r'));
  WriteByte(static_cast<std::byte>('\n'));
  return StatusWithSize(OkStatus(), s.size() + 2);
}

}  // namespace pw::sys_io
