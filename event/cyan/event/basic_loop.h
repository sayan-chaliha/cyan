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
#include <thread>

#include <cyan/noncopyable.h>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits>
class basic_loop : public cyan::noncopyable {
public:
  using backend_traits_type = typename BackendTraits::loop;
  using native_handle_type = typename backend_traits_type::native_handle_type;

  basic_loop();
  explicit basic_loop(basic_loop&& other) noexcept;
  ~basic_loop();

  basic_loop& operator =(basic_loop&& other) noexcept;

  native_handle_type native_handle() const noexcept;

  void start() noexcept;
  void stop() noexcept;
  std::thread::id get_owner_thread_id() const noexcept;

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
      void (*)(native_handle_type)> native_handle_;
  std::thread::id owner_thread_;
};

} // v1
} // cyan::event
