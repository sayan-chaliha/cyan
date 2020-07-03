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
#include <cyan/event.h>
#include <cyan/dispatch/message.h>

namespace cyan::dispatch::detail {

message::message(message::type_t type) : type_{ type }, arrival_time_{ std::chrono::steady_clock::now() } {
}

message::type_t message::get_type() const {
  return type_;
}

void message::set_timeout(std::chrono::milliseconds const& timeout) {
  timeout_ = timeout;
}

std::chrono::milliseconds message::get_timeout() const {
  return timeout_;
}

void message::set_serial_token(cyan::dispatch::serial_token const& token) {
  serial_token_.emplace(token);
  sequence_ = token.get_next_sequence();
}

bool message::try_acquire() {
  if (serial_token_.has_value()) return serial_token_.value().try_acquire(sequence_);
  return true;
}

void message::release() {
  if (serial_token_.has_value()) return serial_token_.value().release();
}

std::chrono::steady_clock::time_point message::arrival_time() const {
  return arrival_time_;
}

std::chrono::milliseconds message::idle_time() const {
  return idle_time_;
}

std::chrono::milliseconds message::processing_time() const {
  return processing_time_;
}

std::chrono::milliseconds message::turnaround_time() const {
  return turnaround_time_;
}

void message::mark_begin_processing() const {
  begin_processing_time_ = std::chrono::steady_clock::now();
  idle_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(begin_processing_time_ - arrival_time_) - timeout_;
}

void message::mark_end_processing() const {
  auto now = std::chrono::steady_clock::now();
  processing_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin_processing_time_);
  turnaround_time_ = processing_time_ + idle_time_;
}

void payload_message::process(cyan::dispatch::handler& handler) {
  mark_begin_processing();
  try {
    handler.on_message(std::move(payload_));
  } catch (std::exception& e) {
    handler.on_error(e);
  }
  mark_end_processing();
}

stop_message::stop_message() : message{ message::type_t::stop } {
}

void stop_message::process(cyan::dispatch::handler&) {
  cyan::this_thread::get_event_loop()->stop();
}

std::unique_ptr<message>
make_stop_message(std::chrono::milliseconds timeout) {
  std::unique_ptr<message> msg = std::unique_ptr<message>{ new stop_message{} };
  msg->set_timeout(timeout);
  return msg;
}

} // cyan::dispatch::detail
