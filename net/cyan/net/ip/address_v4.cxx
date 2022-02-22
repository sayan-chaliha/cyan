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

#include <cyan/net/ip/address_v4.h>

namespace cyan::net::ip {

address_v4::address_v4() noexcept {
  addr_.s_addr = 0;
}

address_v4::address_v4(uint_type addr) noexcept {
  addr_.s_addr = net::detail::host_to_network_long(addr);
}

address_v4::uint_type address_v4::to_uint() const noexcept {
  return net::detail::network_to_host_long(addr_.s_addr);
}

std::string address_v4::to_string() const {
  std::error_code ec;
  auto addr = net::detail::inet_ntop(CYAN_OS_DEF(AF_INET), &addr_, ec);
  if (ec) throw std::system_error{ ec };
  return addr;
}

bool address_v4::is_unspecified() const noexcept {
  return addr_.s_addr == 0;
}

bool address_v4::is_loopback() const noexcept {
  return (to_uint() & 0xFF000000) == 0x7F000000;
}

bool address_v4::is_multicast() const noexcept {
  return (to_uint() & 0xF0000000) == 0xE0000000;
}

address_v4 make_address_v4(std::string_view addr_str) {
  std::error_code ec;
  address_v4 addr = make_address_v4(addr_str, ec);
  if (ec) throw std::system_error{ ec };
  return addr;
}

address_v4 make_address_v4(std::string_view addr_str, std::error_code& ec) {
  address_v4 addr;
  net::detail::inet_pton(CYAN_OS_DEF(AF_INET), addr_str, &addr.addr_, ec);
  return addr;
}

} // cyan::net::ip
