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
basic_io<BackendTraits>::basic_io(std::weak_ptr<loop_type> const& loop) : base{ loop },
      native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate } {
}

template<typename BackendTraits>
basic_io<BackendTraits>::basic_io(basic_io&& other) noexcept : base{ std::move(other) },
      native_handle_{ std::move(other.native_handle_) } {
}

template<typename BackendTraits>
basic_io<BackendTraits>::~basic_io() {
  base::stop();
}

template<typename BackendTraits>
basic_io<BackendTraits>&  basic_io<BackendTraits>::operator =(basic_io&& other) noexcept {
  base::operator =(std::move(other));
  native_handle_ = std::move(other.native_handle_);
  return *this;
}

template<typename BackendTraits>
typename basic_io<BackendTraits>::native_handle_type basic_io<BackendTraits>::native_handle() const {
  return native_handle_.get();
}

template<typename BackendTraits>
void basic_io<BackendTraits>::set_event_flags(event_flags flags) noexcept {
  backend_traits_type::set_event_flags(native_handle_.get(), flags);
}

template<typename BackendTraits>
typename basic_io<BackendTraits>::event_flags basic_io<BackendTraits>::get_event_flags() const noexcept {
  return backend_traits_type::get_event_flags(native_handle_.get());
}

template<typename BackendTraits>
void basic_io<BackendTraits>::set_file_descriptor(std::int32_t fd) noexcept {
  backend_traits_type::set_file_descriptor(native_handle_.get(), fd);
}

template<typename BackendTraits>
std::int32_t basic_io<BackendTraits>::get_file_descriptor() const noexcept {
  return backend_traits_type::get_file_descriptor(native_handle_.get());
}

} // v1
} // cyan::event
