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
  using callback_type = typename backend_traits_type::callback_type;

  constexpr static event_flags event_read = backend_traits_type::event_read;
  constexpr static event_flags event_write = backend_traits_type::event_write;
  constexpr static event_flags event_error = backend_traits_type::event_error;

  basic_io(std::weak_ptr<loop_type> const& loop);
  explicit basic_io(basic_io&& other) noexcept;
	~basic_io();

  basic_io& operator =(basic_io&& other) noexcept;

  native_handle_type native_handle() const;

  void set_event_flags(event_flags flags) noexcept;
  event_flags get_event_flags() const noexcept;
  void set_file_descriptor(std::int32_t fd) noexcept;
  std::int32_t get_file_descriptor() const noexcept;
  void set_callback(callback_type&& callback) noexcept;

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
      void (*)(native_handle_type)> native_handle_;
};

} // v1
} // cyan::event
