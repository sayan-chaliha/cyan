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

#include <cyan/noncopyable.h>
#include <cyan/event/basic_loop.h>

namespace cyan::event {

template<typename T>
class basic_timer : public cyan::noncopyable {
public:
  using backend_traits_type = T;
  using native_handle_type = typename backend_traits_type::timer::native_handle_type;

  basic_timer(std::weak_ptr<basic_loop<backend_traits_type>> const& loop_ref) : loop_ref_{ loop_ref },
        native_handle_{ backend_traits_type::timer::allocate(), backend_traits_type::timer::deallocate } {
  }

  explicit basic_timer(basic_timer&& other) : loop_ref_{ std::move(other.loop_ref_) },
        native_handle_{ std::move(other.native_handle_) } {
    other.loop_ref_ = nullptr;
    other.native_handle_ = nullptr;
  }

  ~basic_timer() {
		if (native_handle_) stop();
  }

  void start() {
    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::timer::start(loop->native_handle(), native_handle_.get());
    }
  }

  void reset() {
    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::timer::reset(loop->native_handle(), native_handle_.get());
    }
  }

  void stop() {
    if (auto loop = loop_ref_.lock()) {
      backend_traits_type::timer::stop(loop->native_handle(), native_handle_.get());
    }
  }

  template<typename U, typename V>
  void set_timeout(std::chrono::duration<U, V> const& t) {
    backend_traits_type::timer::set_timeout(native_handle_.get(), t);
  }

  std::chrono::milliseconds get_timeout() const {
    return backend_traits_type::timer::get_timeout(native_handle_.get());
  }

  bool is_active() const {
    return backend_traits_type::timer::is_active(native_handle_.get());
  }

  bool is_pending() const {
    return backend_traits_type::timer::is_pending(native_handle_.get());
  }

  template<typename F, typename... Args>
  void set_callback(F&& f, Args&&... args) {
    backend_traits_type::timer::set_callback(native_handle_.get(), std::forward<F>(f),
					std::forward<Args>(args)...);
  }

private:
  std::weak_ptr<basic_loop<backend_traits_type>> loop_ref_;
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
      void (*)(native_handle_type)> native_handle_;
};

} // cyan::event
