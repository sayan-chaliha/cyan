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


namespace cyan {

namespace event {

template<typename T>
class basic_loop;

template<typename T>
std::shared_ptr<event::basic_loop<T>> get_main_loop();

}

namespace this_thread {

template<typename T>
std::shared_ptr<event::basic_loop<T>> get_event_loop();

}

namespace event {

template<typename T>
class basic_loop : public cyan::noncopyable {
public:
  using backend_traits_type = T;
  using native_handle_type = typename backend_traits_type::loop::native_handle_type;

  ~basic_loop() {
    stop();
  }

  explicit basic_loop(basic_loop&& other) : native_handle_{ std::move(other.native_handle_) } {
    other.native_handle_ = nullptr;
  }

  basic_loop& operator =(basic_loop&& other) {
    native_handle_ = other.native_handle_;
    other.native_handle_ = nullptr;
    return *this;
  }

  native_handle_type native_handle() const {
    return native_handle_.get();
  }

  void start() {
    backend_traits_type::loop::start(native_handle_.get());
  }

  void stop() {
    backend_traits_type::loop::stop(native_handle_.get());
  }

  std::thread::id get_owner_thread_id() const {
    return owner_thread_;
  }

protected:
  friend std::shared_ptr<basic_loop<T>> cyan::event::get_main_loop();
  friend std::shared_ptr<basic_loop<T>> cyan::this_thread::get_event_loop();

  basic_loop() : native_handle_{ backend_traits_type::loop::allocate(), backend_traits_type::loop::deallocate },
        owner_thread_{ std::this_thread::get_id() } {
  }

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
      void (*)(native_handle_type)> native_handle_;
  std::thread::id owner_thread_;
};

} // event

} // cyan
