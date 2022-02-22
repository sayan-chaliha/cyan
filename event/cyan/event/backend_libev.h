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

#include <functional>

#include <cyan/event/backend.h>
#include <cyan/external/ev/ev.h>

namespace cyan::event {

namespace backend {

struct libev {};

} // backend

// TODO: Find a place to call ev_syserror_cb()

template<>
struct backend_traits<backend::libev> {
  struct loop {
    using native_handle_type = struct ::ev_loop*;

    static native_handle_type allocate();
    static void deallocate(native_handle_type loop);
    static void start(native_handle_type loop) noexcept;
    static void stop(native_handle_type loop) noexcept;
  };

  struct async {
  public:
    using native_handle_type = struct ::ev_async*;
    using callback_type = std::function<void()>;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    
    static void start(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void send(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;
    static void set_callback(native_handle_type native_handle, callback_type&&) noexcept;
  
  private:
    static void async_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct timer {
	public:
    using native_handle_type = struct ::ev_timer*;
    using callback_type = std::function<void()>;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void reset(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;

    template<typename U, typename V>
    static void set_timeout(native_handle_type native_handle, std::chrono::duration<U, V> const& t) noexcept {
      auto timeout_msec = std::chrono::duration_cast<std::chrono::milliseconds>(t);
      double timeout_s = double(timeout_msec.count()) / 1000.;
      set_timeout(native_handle, timeout_s);
    }

    static std::chrono::milliseconds get_timeout(native_handle_type native_handle) noexcept;
    static void set_callback(native_handle_type native_handle, callback_type&&) noexcept;

  private:
    static void set_timeout(native_handle_type native_handle, double secs) noexcept;
    static void timer_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct signal {
	public:
    using native_handle_type = struct ::ev_signal*;
    using callback_type = std::function<void()>;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void set_number(native_handle_type native_handle, std::int32_t signum) noexcept;
    static std::int32_t get_number(native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;
    static void set_callback(native_handle_type native_handle, callback_type&&) noexcept;

  private:
    static void signal_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct idle {
	public:
    using native_handle_type = struct ::ev_idle*;
    using callback_type = std::function<void()>;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;
    static void set_callback(native_handle_type native_handle, callback_type&&) noexcept;

  private:
    static void idle_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct io {
  public:
    using event_flags = std::int32_t;
    using native_handle_type = struct ::ev_io*;
    using callback_type = std::function<void(event_flags)>;

    constexpr static event_flags event_read = EV_READ;
    constexpr static event_flags event_write = EV_WRITE;
    constexpr static event_flags event_error = EV_ERROR;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;
    static void set_file_descriptor(native_handle_type naitve_handle, std::int32_t fd) noexcept;
    static std::int32_t get_file_descriptor(native_handle_type native_handle) noexcept;
    static void set_event_flags(native_handle_type native_handle, event_flags ev) noexcept;
    static event_flags get_event_flags(native_handle_type native_handle) noexcept;
    static void set_callback(native_handle_type native_handle, callback_type&&) noexcept;

  private:
    static void io_callback(typename loop::native_handle_type, native_handle_type native_handle, int revents) noexcept;
  };

};

} // cyan::event
