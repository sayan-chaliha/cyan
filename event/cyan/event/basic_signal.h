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

#include <cyan/event/event_base.h>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits>
class basic_signal : public cyan::event::event_base<BackendTraits, basic_signal> {
private:
  using base = cyan::event::event_base<BackendTraits, basic_signal>;

public:
  using backend_traits_type = typename BackendTraits::signal;
  using native_handle_type = typename backend_traits_type::native_handle_type;
  using loop_type = typename base::loop_type;
  using callback_type = typename backend_traits_type::callback_type;

  basic_signal(std::weak_ptr<loop_type> const& loop) : base{ loop },
        native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate } {
  }

  explicit basic_signal(basic_signal&& other) noexcept : base{ std::move(other) },
        native_handle_{ std::move(other.native_handle_) } {
    other.native_handle_ = nullptr;
  }

	~basic_signal() {
    base::stop();
  }

  basic_signal& operator =(basic_signal&& other) noexcept {
    base::operator =(std::move(other));
    native_handle_ = std::move(other.native_handle_);
    other.native_handle_ = nullptr;
    return *this;
  }

  native_handle_type native_handle() const {
    return native_handle_.get();
  }

  std::int32_t get_number() const noexcept {
    return backend_traits_type::get_number(native_handle_.get());
  }

  void set_number(std::int32_t signum) noexcept {
    bool const was_active = base::is_active();
    base::stop();
    backend_traits_type::set_number(native_handle_.get(), signum);
    if (was_active) {
      base::start();
    }
  }

  void set_callback(callback_type&& callback) {
    backend_traits_type::set_callback(native_handle_.get(), std::forward<callback_type>(callback));
  }

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
        void (*)(native_handle_type)> native_handle_;
};

} // v1
} // cyan::event
