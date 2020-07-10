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

#include <cyan/callable.h>
#include <cyan/event/backend.h>
#include <cyan/external/ev/ev.h>

namespace cyan::event {

namespace backend {

struct libev {};

} // backend

namespace detail {

struct state {
  virtual ~state() = default;
  virtual void invoke_callback() = 0;
};

template<typename C>
struct state_impl : public state {
  state_impl(C&& c) : callable_{ std::forward<C>(c) } {}
  void invoke_callback() override { callable_(); }

private:
  C callable_;
};
		
template<typename F, typename ...Args>
static state* make_state(F&& f, Args&&... args) {
  return new state_impl{ cyan::make_callable(std::forward<F>(f), std::forward<Args>(args)...) };
}

} // detail

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

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    
    template<typename F, typename ...Args>
    static void set_callback(native_handle_type native_handle, F&& f, Args&&... args) {
      if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
      native_handle->data = detail::make_state(std::forward<F>(f), std::forward<Args>(args)...);
    }

    static void start(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static void send(typename loop::native_handle_type loop, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;
  
  private:
    static void async_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct timer {
	public:
    using native_handle_type = struct ::ev_timer*;

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

    template<typename F, typename ...Args>
    static void set_callback(native_handle_type native_handle, F&& f, Args&&... args) {
      if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
      native_handle->data = detail::make_state(std::forward<F>(f), std::forward<Args>(args)...);
    }

  private:
    static void set_timeout(native_handle_type native_handle, double secs) noexcept;
    static void timer_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct signal {
	public:
    using native_handle_type = struct ::ev_signal*;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void set_number(native_handle_type native_handle, std::int32_t signum) noexcept;
    static std::int32_t get_number(native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;

    template<typename F, typename ...Args>
    static void set_callback(native_handle_type native_handle, F&& f, Args&&... args) {
      if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
      native_handle->data = detail::make_state(std::forward<F>(f), std::forward<Args>(args)...);
    }

  private:
    static void signal_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct idle {
	public:
    using native_handle_type = struct ::ev_idle*;

    static native_handle_type allocate();
    static void deallocate(native_handle_type native_handle);
    static void start(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static void stop(typename loop::native_handle_type native_loop_handle, native_handle_type native_handle) noexcept;
    static bool is_active(native_handle_type native_handle) noexcept;
    static bool is_pending(native_handle_type) noexcept;

    template<typename F, typename ...Args>
    static void set_callback(native_handle_type native_handle, F&& f, Args&&... args) {
      if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
      native_handle->data = detail::make_state(std::forward<F>(f), std::forward<Args>(args)...);
    }

  private:
    static void idle_callback(typename loop::native_handle_type, native_handle_type native_handle, int) noexcept;
  };

  struct io {
  public:
    using event_flags = std::int32_t;
    constexpr static event_flags event_read = EV_READ;
    constexpr static event_flags event_write = EV_WRITE;
    constexpr static event_flags event_error = EV_ERROR;

  private:
    struct state {
      virtual ~state() = default;
      virtual void invoke_callback(event_flags) = 0;
    };
    
    template<typename C>
    struct state_impl : public state {
      state_impl(C&& c) : callable_{ std::forward<C>(c) } {}
      void invoke_callback(event_flags flags) override { callable_(flags); }
    
    private:
      C callable_;
    };
		
    template<typename F, typename ...Args>
    static state* make_state(F&& f, Args&&... args) {
      return new state_impl{ cyan::make_callable(std::forward<F>(f), std::forward<Args>(args)...) };
    }

	public:
    using native_handle_type = struct ::ev_io*;

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

    template<typename F, typename ...Args>
    static void set_callback(native_handle_type native_handle, F&& f, Args&&... args) {
      if (native_handle->data) delete static_cast<state*>(native_handle->data);
      native_handle->data = make_state(std::forward<F>(f), std::forward<Args>(args)...);
    }

  private:
    static void io_callback(typename loop::native_handle_type, native_handle_type native_handle, int revents) noexcept;
  };

};

} // cyan::event
