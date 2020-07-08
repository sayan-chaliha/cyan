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

#include <cyan/net/ip/address.h>

namespace cyan::net::ip::detail {

class endpoint {
public:
  endpoint() noexcept;
  endpoint(std::int32_t family, std::uint16_t port) noexcept;
  endpoint(cyan::net::ip::address const& addr, std::uint16_t port) noexcept;

  endpoint& operator =(endpoint const&) = default;
  endpoint& operator =(endpoint&&) = default;

  std::uint16_t get_port() const noexcept;
  void set_port(std::uint16_t port) noexcept;

  bool is_v4() const noexcept {
    return addr_.base.sa_family == CYAN_OS_DEF(AF_INET);
  }

  bool is_v6() const noexcept {
    return addr_.base.sa_family == CYAN_OS_DEF(AF_INET6);
  }

  cyan::net::ip::address get_address() const noexcept;
  void set_address(cyan::net::ip::address const& addr) noexcept;

  cyan::net::detail::sockaddr_type const* data() const noexcept {
    return &addr_.base;
  }

  cyan::net::detail::sockaddr_type* data() noexcept {
    return &addr_.base;
  }

  std::size_t size() const noexcept {
    if (is_v4()) return sizeof(addr_.v4);
    else return sizeof(addr_.v6);
  }

  std::size_t capacity() const noexcept {
    return sizeof(addr_.storage);
  }

  std::string to_string() const;

private:
  union {
    cyan::net::detail::sockaddr_type base;
    cyan::net::detail::sockaddr_in4_type v4;
    cyan::net::detail::sockaddr_in6_type v6;
    cyan::net::detail::sockaddr_storage_type storage;
  } addr_;
};

}
