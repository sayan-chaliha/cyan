
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

#include <cyan/noncopyable.h>
#include <cyan/event.h>
#include <cyan/net/socket_base.h>

namespace cyan::net {

template<typename Protocol>
class basic_socket : public socket_base, protected cyan::event::io {
private:
  using base_io = cyan::event::io;
  using event_loop_type = std::weak_ptr<cyan::event::loop> const&;

public:
  using protocol_type = Protocol;
  using native_handle_type = cyan::net::detail::socket_type;
  using endpoint_type = typename protocol_type::endpoint;

  basic_socket(event_loop_type loop_ref) : base_io{ loop_ref },
        native_handle_{ cyan::net::detail::invalid_socket } {
  }

  basic_socket(event_loop_type loop_ref, native_handle_type native_handle) noexcept
        : base_io{ loop_ref }, native_handle_{ native_handle } {
    base_io::set_file_descriptor(native_handle_);
  }

  basic_socket(event_loop_type loop_ref, protocol_type const& protocol) : base_io{ loop_ref },
        native_handle_{ cyan::net::detail::invalid_socket } {
    open(protocol);
  }

  basic_socket(event_loop_type loop_ref, endpoint_type const& endpoint) : base_io{ loop_ref },
        native_handle_{ cyan::net::detail::invalid_socket } {
    bind(endpoint);
  }

  basic_socket(basic_socket&& other) noexcept : base_io{ std::move(other) },
        native_handle_{ other.native_handle_ },
        local_endpoint_{ std::move(other.local_endpoint_) },
        remote_endpoint_{ std::move(other.remote_endpoint_) } {
    other.native_handle_ = cyan::net::detail::invalid_socket;
  }

  basic_socket& operator =(basic_socket&& other) noexcept {
    base_io::operator =(std::move(other));
    native_handle_ = other.native_handle_;
    other.native_handle_ = cyan::net::detail::invalid_socket;
    local_endpoint_ = std::move(other.local_endpoint_);
    remote_endpoint_ = std::move(other.remote_endpoint_);
    return *this;
  }

  native_handle_type native_handle() const {
    return native_handle_;
  }

  void open(protocol_type const& protocol = protocol_type()) {
    std::error_code ec;

    native_handle_ = cyan::net::detail::socket(protocol.family(), protocol.type(), protocol.protocol(), ec);
    if (ec) throw std::system_error{ ec };

    base_io::set_file_descriptor(native_handle_);
  }

  bool is_open() const noexcept {
    return native_handle_ != cyan::net::detail::invalid_socket;
  }

  void close() {
    if (!is_open()) return;

    std::error_code ec;

    base_io::stop();
    cyan::net::detail::close(native_handle_, ec);
    if (ec) throw std::system_error{ ec };

    native_handle_ = cyan::net::detail::invalid_socket;
    local_endpoint_ = endpoint_type{};
    remote_endpoint_ = endpoint_type{};
    base_io::set_file_descriptor(native_handle_);
  }

  void shutdown(socket_base::shutdown_type how) {
    std::error_code ec;
    cyan::net::detail::shutdown(native_handle_, static_cast<int>(how), ec);
    if (ec) throw std::system_error{ ec };
  }

  std::size_t available() const {
    return 0;
  }

  void bind(endpoint_type const& endpoint, bool reuse_addr = true) {
    if (!is_open()) open(endpoint.protocol());

    set_option(socket_base::reuse_address{ reuse_addr });

    std::error_code ec;
    cyan::net::detail::bind(native_handle_, endpoint.data(), endpoint.size(), ec);
    if (ec) throw std::system_error{ ec };
  }

  void connect(endpoint_type const& peer) {
    std::error_code ec;
    cyan::net::detail::connect(native_handle_, peer.data(), peer.size(), ec);
    if (ec) throw std::system_error{ ec };
    remote_endpoint_ = peer;
  }

  template<typename Option>
  void set_option(Option const& option) {
    std::error_code ec;
    cyan::net::detail::set_socket_option(native_handle_, option, ec);
    if (ec) throw std::system_error{ ec };
  }

  template<typename Option>
  void get_option(Option& option) {
    std::error_code ec;
    cyan::net::detail::get_socket_option(native_handle_, option, ec);
    if (ec) throw std::system_error{ ec };
  }

  bool is_non_blocking() const {
    std::error_code ec;
    std::int32_t result = cyan::net::detail::socket_get_non_blocking(native_handle_, ec);
    if (result < 0) throw std::system_error{ ec };
    return result;
  }

  bool set_non_blocking(bool non_blocking) const {
    std::error_code ec;
    std::int32_t result = cyan::net::detail::socket_set_non_blocking(native_handle_, non_blocking, ec);
    if (result < 0) throw std::system_error{ ec };
    return result;
  }

  endpoint_type local_endpoint() const {
    if (local_endpoint_.is_unspecified()) {
      std::uint32_t size = local_endpoint_.capacity();
      std::error_code ec;
      cyan::net::detail::get_sock_name(native_handle_, local_endpoint_.data(), &size, ec);
      if (ec) throw std::system_error{ ec };
    }
    return local_endpoint_;
  }

  endpoint_type remote_endpoint() const {
    if (remote_endpoint_.is_unspecified()) {
      std::uint32_t size = remote_endpoint_.capacity();
      std::error_code ec;
      cyan::net::detail::get_peer_name(native_handle_, remote_endpoint_.data(), &size, ec);
      if (ec) throw std::system_error{ ec };
    }
    return remote_endpoint_;
  }

  void start() noexcept {
    base_io::start();
  }

  void stop() noexcept {
    base_io::stop();
  }

protected:
  ~basic_socket() {
    close();
  }

  native_handle_type native_handle_;
  mutable endpoint_type local_endpoint_;
  mutable endpoint_type remote_endpoint_;
};

} // cyan::net
