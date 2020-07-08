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
