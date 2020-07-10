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

#include <cstdint>
#include <chrono>

namespace cyan::event {

struct native_handle_types {
  using loop_handle_type = void*;
  using timer_handle_type = void*;
  using timer_callback_type = void (*)();
  using async_handle_type = void*;
  using async_callback_type = void (*)();
  using signal_handle_type = void*;
  using signal_callback_type = void (*)(std::int32_t);
  using idle_handle_type = void*;
  using idle_callback_type = void (*)();
  using io_handle_type = void*;
  using io_callback_type = void (*)(std::int32_t);
};

template<typename T>
struct backend_traits {
  struct loop {
    using native_handle_type = typename T::loop_handle_type;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(native_handle_type) noexcept {
    }

    static void stop(native_handle_type) noexcept {
    }
  };

  struct async {
    using native_handle_type = typename T::async_handle_type;
    using callback_type = typename T::async_callback_type;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void stop(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void send(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void set_callback(native_handle_type, callback_type const&) noexcept {
    }

    static bool is_active(native_handle_type) noexcept {
      return false;
    }

    static bool is_pending(native_handle_type) noexcept {
      return false;
    }
  };

  struct timer {
    using native_handle_type = typename T::timer_handle_type;
    using callback_type = typename T::timer_callback_type;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void reset(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void stop(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    template<typename U, typename V>
    static void set_timeout(native_handle_type, std::chrono::duration<U, V> const&) noexcept {
    }

    static std::chrono::milliseconds get_timeout(native_handle_type) noexcept {
    }

    static void set_callback(native_handle_type, callback_type const&) noexcept {
    }

    static bool is_active(native_handle_type) noexcept {
      return false;
    }

    static bool is_pending(native_handle_type) noexcept {
      return false;
    }
  };

  struct signal {
    using native_handle_type = typename T::signal_handle_type;
    using callback_type = typename T::signal_callback_type;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void stop(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void set_number(native_handle_type, std::int32_t) noexcept {
    }

    static std::int32_t get_number(native_handle_type) noexcept {
      return -1;
    }

    static void set_callback(native_handle_type, callback_type const&) noexcept {
    }

    static bool is_active(native_handle_type) noexcept {
      return false;
    }

    static bool is_pending(native_handle_type) noexcept {
      return false;
    }
  };

  struct idle {
    using native_handle_type = typename T::idle_handle_type;
    using callback_type = typename T::idle_callback_type;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void stop(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void set_callback(native_handle_type, callback_type const&) noexcept {
    }

    static bool is_active(native_handle_type) noexcept {
      return false;
    }

    static bool is_pending(native_handle_type) noexcept {
      return false;
    }
  };

  struct io {
    using native_handle_type = typename T::io_handle_type;
    using callback_type = typename T::io_callback_type;
    using event_flags = std::int32_t;
    constexpr static event_flags event_read = 0;
    constexpr static event_flags event_write = 0;
    constexpr static event_flags event_error = 0;

    static native_handle_type allocate() {
      return native_handle_type();
    }

    static void deallocate(native_handle_type) {
    }

    static void start(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void stop(typename loop::native_handle_type, native_handle_type) noexcept {
    }

    static void set_file_descriptor(std::int32_t) noexcept {
    }

    static std::int32_t get_file_descriptor() noexcept {
      return -1;
    }

    static void set_callback(native_handle_type, callback_type const&) noexcept {
    }

    static bool is_active(native_handle_type) noexcept {
      return false;
    }

    static bool is_pending(native_handle_type) noexcept {
      return false;
    }
  };
};

} // cyan::event::backend

