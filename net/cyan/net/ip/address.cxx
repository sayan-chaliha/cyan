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
#include <string>

#include <cyan/net/ip/address.h>

namespace cyan::net::ip {

address::address() noexcept : type_{ ipv4 } {
}

address::address(address_v4 const& addr_v4) noexcept : type_{ ipv4 }, addr_v4_{ addr_v4 } {
}

address::address(address_v4&& addr_v4) noexcept : type_{ ipv4 }, addr_v4_{ std::forward<address_v4>(addr_v4) } {
}

address& address::operator =(address_v4 const& addr_v4) noexcept {
  type_ = ipv4;
  addr_v4_ = addr_v4;
  return *this;
}

address& address::operator =(address_v4&& addr_v4) noexcept {
  type_ = ipv4;
  addr_v4_ = std::move(addr_v4);
  return *this;
}

address::uint_type address::to_uint() const noexcept {
  if (is_v4()) return addr_v4_.to_uint();
  return 0;
}

std::string address::to_string() const {
  if (is_v4()) return addr_v4_.to_string();
  else return "";
}

bool address::is_v4() const noexcept {
  return type_ == ipv4;
}

bool address::is_v6() const noexcept {
  return type_ == ipv6;
}

bool address::is_unspecified() const noexcept {
  if (is_v4()) return addr_v4_.is_unspecified();
  return false;
}

bool address::is_loopback() const noexcept {
  if (is_v4()) return addr_v4_.is_loopback();
  return false;
}

bool address::is_multicast() const noexcept {
  if (is_v4()) return addr_v4_.is_multicast();
  return false;
}

address make_address(std::string_view addr_str) {
  std::error_code ec;
  address addr = make_address(addr_str, ec);
  if (ec) throw std::system_error{ ec };
  return addr;
}

address make_address(std::string_view addr_str, std::error_code& ec) {
  address addr = make_address_v4(addr_str, ec);
  return addr;
}

} // cyan::net::ip
