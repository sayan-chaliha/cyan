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

#include <cyan/event/event_base.h>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits>
class basic_async : public cyan::event::event_base<BackendTraits, basic_async> {
private:
  using base = cyan::event::event_base<BackendTraits, basic_async>;
  using loop_type = typename base::loop_type;

public:
  using backend_traits_type = typename BackendTraits::async;
  using native_handle_type = typename backend_traits_type::native_handle_type;
  using callback_type = typename backend_traits_type::callback_type;

  basic_async(std::weak_ptr<loop_type> const& loop);
  explicit basic_async(basic_async&& other) noexcept;
  ~basic_async();

  basic_async& operator =(basic_async&& other) noexcept;

  native_handle_type native_handle() const noexcept;

  void send() noexcept;

  void set_callback(callback_type&&) noexcept;

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
        void (*)(native_handle_type)> native_handle_;
  std::atomic<std::uint32_t> count_;
};

} // v1
} // cyan::event
