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

template<typename T>
basic_loop<T>::basic_loop()
      : native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate },
      owner_thread_{ std::this_thread::get_id() } {
}

template<typename T>
basic_loop<T>::~basic_loop() {
  stop();
}

template<typename T>
basic_loop<T>::basic_loop(basic_loop&& other) noexcept : native_handle_{ std::move(other.native_handle_) } {
}

template<typename T>
basic_loop<T>& basic_loop<T>::operator =(basic_loop&& other) noexcept {
  native_handle_ = std::move(other.native_handle_);
  return *this;
}

template<typename T>
typename basic_loop<T>::native_handle_type basic_loop<T>::native_handle() const noexcept {
  return native_handle_.get();
}

template<typename T>
void basic_loop<T>::start() noexcept {
  backend_traits_type::start(native_handle_.get());
}

template<typename T>
void basic_loop<T>::stop() noexcept {
  backend_traits_type::stop(native_handle_.get());
}

template<typename T>
std::thread::id basic_loop<T>::get_owner_thread_id() const noexcept {
  return owner_thread_;
}

} // v1
} // cyan::event
