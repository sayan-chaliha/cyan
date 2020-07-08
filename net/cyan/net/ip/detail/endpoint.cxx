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
#include <cyan/net/ip/detail/endpoint.h>

namespace cyan::net::ip::detail {

endpoint::endpoint() noexcept {
  addr_.v4.sin_family = CYAN_OS_DEF(AF_INET);
  addr_.v4.sin_port = 0;
  addr_.v4.sin_addr.s_addr = CYAN_OS_DEF(INADDR_ANY);
}

endpoint::endpoint(cyan::net::ip::address const& addr, std::uint16_t port) noexcept {
  if (addr.is_v4()) {
    addr_.v4.sin_family = CYAN_OS_DEF(AF_INET);
    addr_.v4.sin_port = cyan::net::detail::host_to_network_short(port);
    addr_.v4.sin_addr.s_addr = cyan::net::detail::host_to_network_long(addr.to_uint());
  } else {
  }
}

endpoint::endpoint(std::int32_t family, std::uint16_t port) noexcept {
  if (family == CYAN_OS_DEF(AF_INET)) {
    addr_.v4.sin_family = family;
    addr_.v4.sin_port = cyan::net::detail::host_to_network_short(port);
    addr_.v4.sin_addr.s_addr = CYAN_OS_DEF(INADDR_ANY);
  } else {
    std::memset(&addr_.v6, 0, sizeof(addr_.v6));
    addr_.v6.sin6_family = family;
    addr_.v6.sin6_port = cyan::net::detail::host_to_network_short(port);
  }
}

std::uint16_t endpoint::get_port() const noexcept {
  if (is_v4()) {
    return cyan::net::detail::network_to_host_short(addr_.v4.sin_port);
  } else {
    return cyan::net::detail::network_to_host_short(addr_.v6.sin6_port);
  }
}

void endpoint::set_port(std::uint16_t port) noexcept {
  if (is_v4()) {
    addr_.v4.sin_port = cyan::net::detail::host_to_network_short(port);
  } else {
    addr_.v6.sin6_port = cyan::net::detail::host_to_network_short(port);
  }
}

cyan::net::ip::address endpoint::get_address() const noexcept {
  if (is_v4()) {
    return cyan::net::ip::address{ cyan::net::ip::address_v4{
          cyan::net::detail::network_to_host_long(addr_.v4.sin_addr.s_addr)  }};
  } else {
    return cyan::net::ip::address{};
  }
}

std::string endpoint::to_string() const {
  return get_address().to_string() + ":" + std::to_string(get_port());
}

void endpoint::set_address(cyan::net::ip::address const& addr) noexcept {
  if (addr.is_v4()) {
    addr_.v4.sin_family = CYAN_OS_DEF(AF_INET);
    addr_.v4.sin_addr.s_addr = cyan::net::detail::host_to_network_long(addr.to_uint());
  } else {
  }
}

}
