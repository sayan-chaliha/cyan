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

#include <chrono>

#include <cyan/net/detail/platform.h>

// level for socket options
#define CYAN_OS_DEF_SOL_SOCKET SOL_SOCKET

// enables recording of debugging information
#define CYAN_OS_DEF_SO_DEBUG SO_DEBUG // boolean

// enables local address reuse
#define CYAN_OS_DEF_SO_REUSEADDR SO_REUSEADDR // boolean

// enables duplicate address and port bindings
#define CYAN_OS_DEF_SO_REUSEPORT SO_REUSEPORT // boolean

// enables keep connections alive
#define CYAN_OS_DEF_SO_KEEPALIVE SO_KEEPALIVE // boolean

// enables routing bypass for outgoing messages
#define CYAN_OS_DEF_SO_DONTROUTE SO_DONTROUTE // boolean

// linger on close if data present
#define CYAN_OS_DEF_SO_LINGER SO_LINGER // linger

// enables permission to transmit broadcast messages
#define CYAN_OS_DEF_SO_BROADCAST SO_BROADCAST // boolean

// enables reception of out-of-band data in band
#define CYAN_OS_DEF_SO_OOBINLINE SO_OOBINLINE // boolean

// set buffer size for output
#define CYAN_OS_DEF_SO_SNDBUF SO_SNDBUF // integer

// set buffer size for input
#define CYAN_OS_DEF_SO_RCVBUF SO_RCVBUF // integer

// set minimum count for output
#define CYAN_OS_DEF_SO_SNDLOWAT SO_SNDLOWAT // integer

// set minimum count for input
#define CYAN_OS_DEF_SO_RCVLOWAT SO_RCVLOWAT // integer

// set timeout value for output
#define CYAN_OS_DEF_SO_SNDTIMEO SO_SNDTIMEO // time

// set timeout value for input
#define CYAN_OS_DEF_SO_RCVTIMEO SO_RCVTIMEO // time

// get the type of the socket (get only)
#define CYAN_OS_DEF_SO_TYPE SO_TYPE // integer

// get and clear error on the socket (get only)
#define CYAN_OS_DEF_SO_ERROR SO_ERROR // integer

#ifdef SO_NOSIGPIPE
// do not generate SIGPIPE, instead return EPIPE
#define CYAN_OS_DEF_SO_NOSIGPIPE SO_NOSIGPIPE // boolean
#endif // SO_NOSIGPIPE

// number of bytes to be read (get only)
#define CYAN_OS_DEF_SO_NREAD SO_NREAD // integer

// number of bytes written not yet sent by the protocol (get only)
#define CYAN_OS_DEF_SO_NWRITE SO_NWRITE // integer

// linger on close if data present with timeout in seconds
#define CYAN_OS_DEF_SO_LINGER_SEC SO_LINGER_SEC

// Disable Nagle's algorithm
#define CYAN_OS_DEF_TCP_NODELAY TCP_NODELAY // boolean

// Maximum segment size
#define CYAN_OS_DEF_TCP_MAXSEG TCP_MAXSEG

// Disable TCP option use
#define CYAN_OS_DEF_TCP_NOOPT TCP_NOOPT

// Delay sending data till connection close or buffer full
#define CYAN_OS_DEF_TCP_NOPUSH TCP_NOPUSH

// Idle time before KEEPALIVE messages are sent
#define CYAN_OS_DEF_TCP_KEEPALIVE TCP_KEEPALIVE

// Connection timeout
#define CYAN_OS_DEF_TCP_CONNECTIONTIMEOUT TCP_CONNECTIONTIMEOUT

// Time between successive keep-alive messages
#define CYAN_OS_DEF_TCP_KEEPINTVL TCP_KEEPINTVL

// Number of keep-alive probes if the receiver is not responding
#define CYAN_OS_DEF_TCP_KEEPCNT TCP_KEEPCNT

namespace cyan::net::detail {

#ifdef HAVE_STRUCT_LINGER
using linger_type = struct ::linger;
#endif // HAVE_STRUCT_LINGER

#ifdef HAVE_STRUCT_TIMEVAL
using timeval_type = struct ::timeval;
#endif // HAVE_STRUCT_TIMEVAL

template<int Level, int Option>
class boolean_socket_option {
public:
  boolean_socket_option(bool value) : value_{ value ? 1 : 0 }, length_{ sizeof(std::int32_t) } {}
  constexpr std::int32_t level() const { return Level; }
  constexpr std::int32_t option() const { return Option; }
  bool get_value() const { return value_ == 1 ? true : false; }
  void set_value(bool value) { value_ = value ? 1 : 0; }
  std::uint32_t length() const { return length_; }
  std::int32_t* data() { return &value_; }
  std::int32_t const* data() const { return &value_; }
  std::uint32_t* data_len() { return &length_; }

private:
  std::int32_t value_;
  std::uint32_t length_;
};

template<int Level, int Option>
class integer_socket_option {
public:
  integer_socket_option(std::int32_t value) : value_{ value }, length_{ sizeof(std::int32_t) }  {}
  constexpr std::int32_t level() const { return Level; }
  constexpr std::int32_t option() const { return Option; }
  std::int32_t get_value() const { return value_; }
  void set_value(std::int32_t value) { value_ = value; }
  std::uint32_t length() const { return length_; }
  std::int32_t* data() { return &value_; }
  std::int32_t const* data() const { return &value_; }
  std::uint32_t* data_len() { return &length_; }

private:
  std::int32_t value_;
  std::uint32_t length_;
};

template<int Level, int Option>
class linger_socket_option {
public:
  linger_socket_option(bool on, std::uint16_t time) {
    value_.l_onoff = on;
    value_.l_linger = time;
    length_ = sizeof(linger_type);
  }

  constexpr std::int32_t level() const { return Level; }
  constexpr std::int32_t option() const { return Option; }
  linger_type get_value() const { return value_; }

  void set_value(bool on, std::uint16_t time) {
    value_.l_onoff = on;
    value_.l_linger = time;
  }

  std::uint32_t length() const { return length_; }
  linger_type* data() { return &value_; }
  linger_type const* data() const { return &value_; }
  std::uint32_t* data_len() { return &length_; }

private:
  linger_type value_;
  std::uint32_t length_;
};

template<int Level, int Option>
class time_socket_option {
public:
  time_socket_option(std::chrono::milliseconds const& time) {
    auto time_usec = time.count();
    value_.tv_sec = time_usec / 1000UL;
    value_.tv_usec = time_usec % 1000UL;
    length_ = sizeof(timeval_type);
  }

  constexpr std::int32_t level() const { return Level; }
  constexpr std::int32_t option() const { return Option; }

  std::chrono::milliseconds get_value() const {
    auto time_usec = (value_.tv_sec * 1000UL) + value_.tv_usec;
    return std::chrono::milliseconds(time_usec);
  }

  void set_value(std::chrono::milliseconds const& time){
    auto time_usec = time.count();
    value_.tv_sec = time_usec / 1000UL;
    value_.tv_usec = time_usec % 1000UL;
  }

  std::uint32_t length() const { return length_; }
  timeval_type* data() { return &value_; }
  timeval_type const* data() const { return &value_; }
  std::uint32_t* data_len() { return &length_; }

private:
  timeval_type value_;
  std::uint32_t length_;
};

template<typename Opt>
std::int32_t set_socket_option(socket_type sock, Opt const& opt, std::error_code& errc) {
  clear_last_error();
  return error_wrapper(::setsockopt(sock, opt.level(), opt.option(), opt.data(), opt.length()), errc);
}

template<typename Opt>
std::int32_t get_socket_option(socket_type sock, Opt& opt, std::error_code& errc) {
  clear_last_error();
  return error_wrapper(::getsockopt(sock, opt.level(), opt.option(), opt.data(), opt.data_len()), errc);
}

}
