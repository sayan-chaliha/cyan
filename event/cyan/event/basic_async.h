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

#include <memory>
#include <atomic>
#include <iostream>

#include <cyan/noncopyable.h>
#include <cyan/event/basic_loop.h>

namespace cyan::event {

template<typename T>
class basic_async : public cyan::noncopyable {
public:
  using backend_traits_type = T;
  using native_handle_type = typename backend_traits_type::async::native_handle_type;

  basic_async(std::weak_ptr<basic_loop<backend_traits_type>> const& loop_ref) : loop_ref_{ loop_ref },
        native_handle_{ backend_traits_type::async::allocate(), backend_traits_type::async::deallocate } {
    count_.store(0, std::memory_order_release);
  }

  explicit basic_async(basic_async&& other) : loop_ref_{ std::move(other.loop_ref_) },
        native_handle_{ std::move(other.native_handle_) } {
    other.loop_ref_ = nullptr;
    other.native_handle_ = nullptr;
  }

  ~basic_async() {
		if (native_handle_) stop();
  }

  void start() {
    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::async::start(loop->native_handle(), native_handle_.get());
    }
  }

  void stop() {
    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::async::stop(loop->native_handle(), native_handle_.get());
    }
  }

  void send() {
    count_.fetch_add(1, std::memory_order_relaxed);

    if (is_pending()) {
      return;
    }

    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::async::send(loop->native_handle(), native_handle_.get());
    }
  }

  bool is_active() const {
    return backend_traits_type::async::is_active(native_handle_.get());
  }

  bool is_pending() const {
    return backend_traits_type::async::is_pending(native_handle_.get());
  }

  template<typename F, typename... Args>
  void set_callback(F&& f, Args&&... args) {
    backend_traits_type::async::set_callback(native_handle_.get(),
          [this, f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
      std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
      if (count_.fetch_sub(1, std::memory_order_acq_rel) > 1) {
        if (auto loop = loop_ref_.lock()) {
          backend_traits_type::async::send(loop->native_handle(), native_handle_.get());
        }
      }
    });
  }

private:
  std::weak_ptr<basic_loop<backend_traits_type>> loop_ref_;
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
        void (*)(native_handle_type)> native_handle_;
  std::atomic<std::uint32_t> count_;
};

} // cyan::event
