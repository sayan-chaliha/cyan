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

#include <cyan/event/basic_async.txx>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits>
basic_async<BackendTraits>::basic_async(std::weak_ptr<loop_type> const& loop) : base{ loop },
      native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate } {
  count_.store(0, std::memory_order_release);
}

template<typename BackendTraits>
basic_async<BackendTraits>::basic_async(basic_async&& other) noexcept : base{ std::move(other) },
      native_handle_{ std::move(other.native_handle_) } {
}

template<typename BackendTraits>
basic_async<BackendTraits>:: ~basic_async() {
  base::stop();
}

template<typename BackendTraits>
basic_async<BackendTraits>& basic_async<BackendTraits>::operator =(basic_async&& other) noexcept {
  base::operator =(std::move(other));
  native_handle_ = std::move(other.native_handle_);
  return *this;
}

template<typename BackendTraits>
typename basic_async<BackendTraits>::native_handle_type basic_async<BackendTraits>::native_handle() const noexcept {
  return native_handle_.get();
}

template<typename BackendTraits>
void basic_async<BackendTraits>::send() noexcept {
  count_.fetch_add(1, std::memory_order_relaxed);

  if (base::is_pending()) {
    return;
  }

  if (auto loop = base::loop_ref_.lock()) {
    backend_traits_type::send(loop->native_handle(), native_handle_.get());
  }
}

template<typename BackendTraits>
void basic_async<BackendTraits>::set_callback(callback_type&& callback) noexcept {
  if (callback) {
    backend_traits_type::set_callback(native_handle_.get(), [this, callback = std::forward<callback_type>(callback)] () mutable {
      callback();

      if (count_.fetch_sub(1, std::memory_order_acq_rel) > 1) {
        send();
      }
    });
  } else {
    backend_traits_type::set_callback(native_handle_.get(), nullptr);
  }
}

} // v1
} // cyan::event
