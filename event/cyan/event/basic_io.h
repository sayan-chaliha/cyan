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
#include <cyan/event/basic_loop.h>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits>
class basic_io : public cyan::event::event_base<BackendTraits, basic_io> {
private:
  using base = cyan::event::event_base<BackendTraits, basic_io>;

public:
  using backend_traits_type = typename BackendTraits::io;
  using native_handle_type = typename backend_traits_type::native_handle_type;
  using loop_type = typename base::loop_type;
  using event_flags = typename backend_traits_type::event_flags;

  constexpr static event_flags event_read = backend_traits_type::event_read;
  constexpr static event_flags event_write = backend_traits_type::event_write;
  constexpr static event_flags event_error = backend_traits_type::event_error;

  basic_io(std::weak_ptr<loop_type> const& loop) : base{ loop },
        native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate } {
  }

  explicit basic_io(basic_io&& other) noexcept : base{ std::move(other) },
        native_handle_{ std::move(other.native_handle_) } {
  }

	~basic_io() {
    base::stop();
  }

  basic_io& operator =(basic_io&& other) noexcept {
    base::operator =(std::move(other));
    native_handle_ = std::move(other.native_handle_);
    return *this;
  }

  native_handle_type native_handle() const {
    return native_handle_.get();
  }

  void set_event_flags(event_flags flags) noexcept {
    backend_traits_type::set_event_flags(native_handle_.get(), flags);
  }

  event_flags get_event_flags() const noexcept {
    return backend_traits_type::get_event_flags(native_handle_.get());
  }

  void set_file_descriptor(std::int32_t fd) noexcept {
    backend_traits_type::set_file_descriptor(native_handle_.get(), fd);
  }

  std::int32_t get_file_descriptor() const noexcept {
    return backend_traits_type::get_file_descriptor(native_handle_.get());
  }

  template<typename F, typename ...Args>
  void set_callback(F&& f, Args&&... args) {
    backend_traits_type::set_callback(native_handle_.get(), std::forward<F>(f),
					std::forward<Args>(args)...);
  }

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
      void (*)(native_handle_type)> native_handle_;
};

}
}
