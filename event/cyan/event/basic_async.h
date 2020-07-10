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

  basic_async(std::weak_ptr<loop_type> const& loop) : base{ loop },
        native_handle_{ backend_traits_type::allocate(), backend_traits_type::deallocate } {
    count_.store(0, std::memory_order_release);
  }

  explicit basic_async(basic_async&& other) noexcept : base{ std::move(other) },
        native_handle_{ std::move(other.native_handle_) } {
  }

  ~basic_async() {
    base::stop();
  }

  basic_async& operator =(basic_async&& other) noexcept {
    base::operator =(std::move(other));
    native_handle_ = std::move(other.native_handle_);
    return *this;
  }

  native_handle_type native_handle() const noexcept {
    return native_handle_.get();
  }

  void send() noexcept {
    count_.fetch_add(1, std::memory_order_relaxed);

    if (base::is_pending()) {
      return;
    }

    if (auto loop = base::loop_ref_.lock()) {
      backend_traits_type::send(loop->native_handle(), native_handle_.get());
    }
  }

  template<typename F, typename... Args>
  void set_callback(F&& f, Args&&... args) {
    backend_traits_type::set_callback(native_handle_.get(),
          [this, f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
      std::invoke(std::forward<F>(f), std::forward<Args>(args)...);

      if (count_.fetch_sub(1, std::memory_order_acq_rel) > 1) {
        if (auto loop = base::loop_ref_.lock()) {
          backend_traits_type::send(loop->native_handle(), native_handle_.get());
        }
      }
    });
  }

private:
  std::unique_ptr<typename std::remove_pointer<native_handle_type>::type,
        void (*)(native_handle_type)> native_handle_;
  std::atomic<std::uint32_t> count_;
};

} // v1
} // cyan::event
