
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

#include <cyan/net/basic_socket.h>

namespace cyan::net::ip {

template<typename Protocol>
class basic_socket_acceptor;

template<typename Protocol>
class basic_stream_socket : public cyan::net::basic_socket<Protocol> {
private:
  using base = cyan::net::basic_socket<Protocol>;
  using event_loop_type = std::weak_ptr<cyan::event::loop> const&;

public:
  using protocol_type = typename base::protocol_type;
  using native_handle_type = typename base::native_handle_type;
  using endpoint_type = typename protocol_type::endpoint;

  basic_stream_socket(event_loop_type loop_ref) : base{ loop_ref } {}

  basic_stream_socket(event_loop_type loop_ref, native_handle_type native_handle) : base{ loop_ref, native_handle } {
  }

  basic_stream_socket(event_loop_type loop_ref, protocol_type const& protocol) : base{ loop_ref, protocol } {
  }

  basic_stream_socket(event_loop_type loop_ref, endpoint_type const& endpoint) : base{ loop_ref, endpoint } {
  }

  basic_stream_socket(basic_stream_socket&&) = default;
  basic_stream_socket& operator =(basic_stream_socket&&) = default;

  ~basic_stream_socket() = default;

  template<typename MutableBuffer>
  std::size_t receive(MutableBuffer&& buffer, socket_base::message_flags flags = 0) {
    std::error_code ec;
    std::int64_t read = cyan::net::detail::receive(base::native_handle_, buffer.data(), buffer.size(), flags, ec);
    if (read < 0 && ec) throw std::system_error{ ec };
    return read;
  }

  template<typename MutableBuffer>
  std::size_t send(MutableBuffer&& buffer, socket_base::message_flags flags = 0) {
    std::error_code ec;
    std::int64_t sent = cyan::net::detail::send(base::native_handle_, buffer.data(), buffer.size(), flags, ec);
    if (sent < 0 && ec) throw std::system_error{ ec };
    return sent;
  }

protected:
  friend class basic_socket_acceptor<Protocol>;
};

} // cyan::net
