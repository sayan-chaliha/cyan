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
#include <cyan/net/detail/address.h>

namespace cyan::net::detail {

constexpr std::size_t max_addr_len = 256;

std::uint16_t host_to_network_short(std::uint16_t num) noexcept {
  return htons(num);
}

std::uint32_t host_to_network_long(std::uint32_t num) noexcept {
  return htonl(num);
}

std::uint16_t network_to_host_short(std::uint16_t num) noexcept {
  return ntohs(num);
}

std::uint32_t network_to_host_long(std::uint32_t num) noexcept {
  return ntohl(num);
}

std::string inet_ntop(std::int32_t af, void const* addr, std::error_code& ec) noexcept {
  char addr_storage[max_addr_len];
  clear_last_error();
  char const* addr_str = error_wrapper(::inet_ntop(af, addr, addr_storage, max_addr_len), ec);
  if (addr_str == nullptr) {
    return "";
  }
  return addr_str;
}

std::int32_t inet_pton(std::int32_t af, std::string_view addr, void* dst, std::error_code& err) noexcept {
  clear_last_error();
  return error_wrapper(::inet_pton(af, addr.data(), dst), err);
}

std::int32_t get_peer_name(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept {
  clear_last_error();
  return error_wrapper(::getpeername(sock, addr, len), ec);
}

std::int32_t get_sock_name(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept {
  clear_last_error();
  return error_wrapper(::getsockname(sock, addr, len), ec);
}

}
