
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
#include <cyan/net/detail/platform.h>
#include <cyan/net/detail/socket_ops.h>
#include <cyan/net/detail/socket_option.h>

namespace cyan::net {

class socket_base : public cyan::noncopyable {
public:
  enum class shutdown_type {
    send = CYAN_OS_DEF(SHUT_WR),
    receive = CYAN_OS_DEF(SHUT_RD),
    both = CYAN_OS_DEF(SHUT_RDWR)
  };

  using message_flags = std::int32_t;
  constexpr static std::int32_t message_peek = CYAN_OS_DEF(MSG_PEEK);
  constexpr static std::int32_t message_out_of_band = CYAN_OS_DEF(MSG_OOB);
  constexpr static std::int32_t message_do_not_route = CYAN_OS_DEF(MSG_DONTROUTE);
  constexpr static std::int32_t message_end_of_record = CYAN_OS_DEF(MSG_EOR);

  using debug = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_DEBUG)>;
  using reuse_address = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_REUSEADDR)>;
  using keep_alive = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_KEEPALIVE)>;
  using do_not_route = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_DONTROUTE)>;
  using linger = cyan::net::detail::linger_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_LINGER)>;
  using broadcast = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_BROADCAST)>;
  using out_of_band_inline = cyan::net::detail::boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_OOBINLINE)>;
  using send_buffer_size = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_SNDBUF)>;
  using receive_buffer_size = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_RCVBUF)>;
  using send_low_watermark = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_SNDLOWAT)>;
  using receive_low_watermark = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_RCVLOWAT)>;
  using send_timeout = cyan::net::detail::time_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_SNDTIMEO)>;
  using receive_timeout = cyan::net::detail::time_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_RCVTIMEO)>;
  using type = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_TYPE)>;
  using error = cyan::net::detail::integer_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_ERROR)>;
  constexpr static std::int32_t max_listen_connections = CYAN_OS_DEF(SOMAXCONN);

protected:
  socket_base() = default;
  ~socket_base() = default;
};

} // cyan::net
