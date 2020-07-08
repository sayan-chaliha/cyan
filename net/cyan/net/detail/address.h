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

#include <string>

#include <cyan/net/detail/platform.h>

#define CYAN_OS_DEF_INADDR_ANY INADDR_ANY

namespace cyan::net::detail {

#ifdef HAVE_STRUCT_SOCKADDR
using sockaddr_type = struct ::sockaddr;
#endif // HAVE_STRUCT_SOCKADDR

#ifdef HAVE_STRUCT_SOCKADDR_STORAGE
using sockaddr_storage_type = struct ::sockaddr_storage;
#endif // HAVE_STRUCT_SOCKADDR_STORAGE

#ifdef HAVE_STRUCT_SOCKADDR_IN
using sockaddr_in4_type = struct ::sockaddr_in;
#endif // HAVE_STRUCT_SOCKADDR_IN

#ifdef HAVE_STRUCT_IN_ADDR
using in4_addr_type = struct ::in_addr;
#endif // HAVE_STRUCT_IN_ADDR

#ifdef HAVE_STRUCT_SOCKADDR_IN6
using sockaddr_in6_type = struct ::sockaddr_in6;
#endif // HAVE_STRUCT_SOCKADDR_IN6

#ifdef HAVE_STRUCT_IN6_ADDR
using in6_addr_type = struct ::in6_addr;
#endif // HAVE_STRUCT_IN6_ADDR

std::uint16_t host_to_network_short(std::uint16_t num) noexcept;
std::uint32_t host_to_network_long(std::uint32_t num) noexcept;
std::uint16_t network_to_host_short(std::uint16_t num) noexcept;
std::uint32_t network_to_host_long(std::uint32_t num) noexcept;
std::string inet_ntop(std::int32_t af, void const* addr, std::error_code& ec) noexcept;
std::int32_t inet_pton(std::int32_t af, std::string_view addr, void* dst, std::error_code& ec) noexcept;
std::int32_t get_peer_name(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept;
std::int32_t get_sock_name(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept;

}
