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

#include <cyan/net/ip/detail/endpoint.h>

namespace cyan::net::ip {

template<typename Protocol>
class basic_endpoint {
public:
  using protocol_type = Protocol;
  using data_type = cyan::net::detail::sockaddr_type;

  basic_endpoint() noexcept = delete;
  basic_endpoint(basic_endpoint const&) noexcept = default;
  basic_endpoint(basic_endpoint&&) noexcept = default;

  basic_endpoint(protocol_type const& protocol, std::uint16_t port) noexcept : impl_{ protocol.family(), port } {
  }

  basic_endpoint(address const& addr, std::uint16_t port) noexcept : impl_{ addr, port } {
  }

  protocol_type protocol() const noexcept {
    if (impl_.is_v4()) return protocol_type::v4();
    else return protocol_type::v6();
  }

  data_type const* data() const noexcept {
    return impl_.data();
  }

  data_type* data() noexcept {
    return impl_.data();
  }

  std::uint16_t get_port() const noexcept {
    return impl_.get_port();
  }

  void set_port(std::uint16_t port) noexcept {
    impl_.set_port(port);
  }

  cyan::net::ip::address get_address() const noexcept {
    return impl_.get_address();
  }

  void set_address(cyan::net::ip::address const& addr) noexcept {
    impl_.set_address(addr);
  }

  std::size_t size() const noexcept {
    return impl_.size();
  }

  std::size_t capacity() const noexcept {
    return impl_.capacity();
  }

private:
  cyan::net::ip::detail::endpoint impl_;
};

template <typename Elem, typename Traits, typename Protocol>
std::basic_ostream<Elem, Traits>& operator <<(
      std::basic_ostream<Elem, Traits>& os,
      const basic_endpoint<Protocol>& endpoint) {
  cyan::net::ip::detail::endpoint tmp_enp{ endpoint.get_address(), endpoint.get_port() };
  return os << tmp_enp.to_string();
}

}
