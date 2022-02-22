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

#include <cyan/net/ip/address_v4.h>

namespace cyan::net::ip {

class address {
public:
  using uint_type = std::uint_least32_t;

  address() noexcept;
  address(address const&) noexcept = default;
  address(address&&) noexcept = default;

  address(address_v4 const& addr_v4) noexcept;
  address(address_v4&& addr_v4) noexcept;

  address& operator =(address const&) noexcept = default;
  address& operator =(address&&)  noexcept = default;

  address& operator =(address_v4 const& addr_v4) noexcept;
  address& operator =(address_v4&& addr_v4) noexcept;

  uint_type to_uint() const noexcept;
  std::string to_string() const;

  bool is_v4() const noexcept;
  bool is_v6() const noexcept;

  bool is_unspecified() const noexcept;
  bool is_loopback() const noexcept;
  bool is_multicast() const noexcept;

private:
  friend address make_address(std::string_view);
  friend address make_address(std::string_view, std::error_code&);

  enum { ipv4, ipv6 } type_;
  cyan::net::ip::address_v4 addr_v4_;
};

address make_address(std::string_view addr);
address make_address(std::string_view addr, std::error_code& ec);

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const address& addr)
{
  return os << addr.to_string().c_str();
}

} // cyan::net::ip
