/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Sayan Chaliha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#pragma once

#include <system_error>

#include <cyan/net/detail/platform.h>
#include <cyan/net/detail/address.h>

#define CYAN_OS_DEF_SHUT_RD SHUT_RD
#define CYAN_OS_DEF_SHUT_WR SHUT_WR
#define CYAN_OS_DEF_SHUT_RDWR SHUT_RDWR

namespace cyan::net::detail {

// -------------------------------------------------------------------------------------------------------------------

socket_type socket(std::int32_t af, std::int32_t type, std::int32_t protocol, std::error_code& ec) noexcept;
socket_type socket_dup(socket_type sock, std::error_code& ec) noexcept;
std::int32_t socket_set_non_blocking(socket_type sock, bool onoff, std::error_code& ec) noexcept;

// -------------------------------------------------------------------------------------------------------------------

std::int32_t bind(socket_type sock, sockaddr_type const* addr, std::uint32_t len, std::error_code& ec) noexcept;
std::int32_t listen(socket_type sock, std::int32_t backlog, std::error_code& ec) noexcept;
std::int32_t connect(socket_type sock, sockaddr_type const* addr, std::uint32_t len, std::error_code& ec) noexcept;
socket_type accept(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept;
std::int32_t shutdown(socket_type sock, std::int32_t how, std::error_code& ec) noexcept;
std::int32_t close(socket_type sock, std::error_code& ec) noexcept;

// -------------------------------------------------------------------------------------------------------------------

}
