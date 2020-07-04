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

#include <any>
#include <chrono>
#include <memory>
#include <optional>
#include <type_traits>

#include <cyan/dispatch/handler.h>
#include <cyan/dispatch/serial_token.h>

namespace cyan::dispatch::detail {

struct message {
  enum class type_t {
    callable,
    payload,
    stop
  };

  virtual ~message() = default;

  message::type_t get_type() const;
  void set_timeout(std::chrono::milliseconds const& timeout);
  std::chrono::milliseconds get_timeout() const;
  void set_serial_token(cyan::dispatch::serial_token const& token);
  bool try_acquire();
  void release();
  std::chrono::steady_clock::time_point arrival_time() const;
  std::chrono::milliseconds idle_time() const;
  std::chrono::milliseconds processing_time() const;
  std::chrono::milliseconds turnaround_time() const;

  virtual void process(cyan::dispatch::handler& handler) = 0;

protected:
  message(message::type_t type);

  void mark_begin_processing() const;
  void mark_end_processing() const;

private:
  message::type_t type_;
  std::chrono::milliseconds timeout_;
  std::optional<cyan::dispatch::serial_token> serial_token_;
  cyan::dispatch::serial_token::sequence_type sequence_;
  mutable std::chrono::steady_clock::time_point arrival_time_;
  mutable std::chrono::steady_clock::time_point begin_processing_time_;
  mutable std::chrono::milliseconds idle_time_;
  mutable std::chrono::milliseconds processing_time_;
  mutable std::chrono::milliseconds turnaround_time_;
};

class payload_message : public message {
public:
  template<typename T>
  payload_message(T&& payload) : message{ message::type_t::payload }, payload_{ std::forward<T>(payload) } {}

  void process(cyan::dispatch::handler& handler) override;

private:
  std::any payload_;
};

template<typename C>
class callable_message : public message {
public:
  static_assert(std::is_invocable_v<C>, "callable_message: type is not invocable");

  callable_message(C&& callable) : message{ message::type_t::callable }, callable_{ std::forward<C>(callable) } {}

  void process(cyan::dispatch::handler& handler) override {
    mark_begin_processing();
    try {
      callable_();
    } catch (std::exception& e) {
      handler.on_error(e);
    }
    release();
    mark_end_processing();
  }

private:
  C callable_;
};

class stop_message : public message {
public:
  stop_message();

  void process(cyan::dispatch::handler&) override;
};

template<typename T>
std::unique_ptr<message>
make_payload_message(T&& payload, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
  std::unique_ptr<message> msg = std::unique_ptr<message>{ new payload_message{ std::forward<T>(payload) } };
  msg->set_timeout(timeout);
  return msg;
}

template<typename C>
std::unique_ptr<message>
make_callable_message(C&& callable, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
  std::unique_ptr<message> msg = std::unique_ptr<message>{ new callable_message<C>{ std::forward<C>(callable) } };
  msg->set_timeout(timeout);
  return msg;
}

std::unique_ptr<message>
make_stop_message(std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

} // cyan::dispatch::detail
