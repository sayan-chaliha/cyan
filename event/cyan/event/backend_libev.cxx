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
#include <cassert>
#include <mutex>

#include <cyan/event/backend_libev.h>

namespace cyan::event {

backend_traits<backend::libev>::loop::native_handle_type
backend_traits<backend::libev>::loop::allocate() {
  auto native_handle = ev_loop_new((EVBACKEND_ALL & ~EVBACKEND_SELECT) | EVFLAG_NOENV);
  assert(native_handle != nullptr && "failed to allocate loop handle");
  ::ev_ref(native_handle);
  return native_handle;
}

void
backend_traits<backend::libev>::loop::deallocate(native_handle_type native_handle) {
  ::ev_loop_destroy(native_handle);
}

void
backend_traits<backend::libev>::loop::start(native_handle_type native_handle) noexcept {
  ::ev_run(native_handle, 0);
}

void
backend_traits<backend::libev>::loop::stop(native_handle_type native_handle) noexcept {
  ::ev_break(native_handle, EVBREAK_ALL);
}

backend_traits<backend::libev>::async::native_handle_type
backend_traits<backend::libev>::async::allocate() {
  auto native_handle = new struct ::ev_async;
  native_handle->data = nullptr;
  ev_async_init(native_handle, async_callback);
  return native_handle;
}
  
void
backend_traits<backend::libev>::async::deallocate(native_handle_type native_handle) {
  if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
  delete native_handle;
}
  
void
backend_traits<backend::libev>::async::start(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_async_start(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::async::stop(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_async_stop(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::async::send(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_async_send(native_loop_handle, native_handle);
}

bool
backend_traits<backend::libev>::async::is_active(native_handle_type native_handle) noexcept {
  return ev_is_active(native_handle);
}

bool
backend_traits<backend::libev>::async::is_pending(native_handle_type native_handle) noexcept {
  return ev_async_pending(native_handle);
}

void
backend_traits<backend::libev>::async::async_callback(typename loop::native_handle_type,
      native_handle_type async, int) noexcept {
  if (async->data) {
    detail::state* s = static_cast<detail::state*>(async->data);
    s->invoke_callback();
  }
}

backend_traits<backend::libev>::timer::native_handle_type
backend_traits<backend::libev>::timer::allocate() {
  auto native_handle = new struct ::ev_timer;
  ev_timer_init(native_handle, timer_callback, 0., 0.);
  native_handle->data = nullptr;
  return native_handle;
}

void
backend_traits<backend::libev>::timer::deallocate(native_handle_type native_handle) {
  if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
  delete native_handle;
}

void
backend_traits<backend::libev>::timer::start(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_timer_again(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::timer::reset(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  start(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::timer::stop(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_timer_stop(native_loop_handle, native_handle);
}

std::chrono::milliseconds
backend_traits<backend::libev>::timer::get_timeout(native_handle_type native_handle) noexcept {
  double msecs = native_handle->repeat * 1000.;
  return std::chrono::milliseconds(int(msecs));
}

bool
backend_traits<backend::libev>::timer::is_active(native_handle_type native_handle) noexcept {
  return ev_is_active(native_handle);
}

bool
backend_traits<backend::libev>::timer::is_pending(native_handle_type native_handle) noexcept {
  return ev_is_pending(native_handle);
}

void
backend_traits<backend::libev>::timer::set_timeout(native_handle_type native_handle, double timeout_s) noexcept {
  native_handle->repeat = timeout_s;
}

void
backend_traits<backend::libev>::timer::timer_callback(typename loop::native_handle_type,
      native_handle_type timer, int) noexcept {
  if (timer->data) {
    detail::state* s = static_cast<detail::state*>(timer->data);
		s->invoke_callback();
  }
}

backend_traits<backend::libev>::signal::native_handle_type
backend_traits<backend::libev>::signal::allocate() {
  auto native_handle = new struct ::ev_signal;
  // initialize with SIGINT by default
  ev_signal_init(native_handle, signal_callback, SIGINT);
  native_handle->data = nullptr;
  return native_handle;
}

void
backend_traits<backend::libev>::signal::deallocate(native_handle_type native_handle) {
  if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
  delete native_handle;
}

void
backend_traits<backend::libev>::signal::start(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_signal_start(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::signal::stop(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_signal_stop(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::signal::set_number(native_handle_type native_handle,
      std::int32_t signum) noexcept {
  ev_signal_init(native_handle, signal_callback, signum);
}

std::int32_t
backend_traits<backend::libev>::signal::get_number(native_handle_type native_handle) noexcept {
  return native_handle->signum;
}

bool
backend_traits<backend::libev>::signal::is_active(native_handle_type native_handle) noexcept {
  return ev_is_active(native_handle);
}

bool
backend_traits<backend::libev>::signal::is_pending(native_handle_type native_handle) noexcept {
  return ev_is_pending(native_handle);
}

void
backend_traits<backend::libev>::signal::signal_callback(typename loop::native_handle_type,
      native_handle_type native_handle, int) noexcept {
  if (native_handle->data) {
    detail::state* s = static_cast<detail::state*>(native_handle->data);
    s->invoke_callback();
  }
}

backend_traits<backend::libev>::idle::native_handle_type
backend_traits<backend::libev>::idle::allocate() {
  auto native_handle = new struct ::ev_idle;
  ev_idle_init(native_handle, idle_callback);
  native_handle->data = nullptr;
  return native_handle;
}

void
backend_traits<backend::libev>::idle::deallocate(native_handle_type native_handle) {
  if (native_handle->data) delete static_cast<detail::state*>(native_handle->data);
  delete native_handle;
}

void
backend_traits<backend::libev>::idle::start(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_idle_start(native_loop_handle, native_handle);
}

void
backend_traits<backend::libev>::idle::stop(typename loop::native_handle_type native_loop_handle,
      native_handle_type native_handle) noexcept {
  ::ev_idle_stop(native_loop_handle, native_handle);
}

bool
backend_traits<backend::libev>::idle::is_active(native_handle_type native_handle) noexcept {
  return ev_is_active(native_handle);
}

bool
backend_traits<backend::libev>::idle::is_pending(native_handle_type native_handle) noexcept {
  return ev_is_pending(native_handle);
}

void
backend_traits<backend::libev>::idle::idle_callback(typename loop::native_handle_type,
      native_handle_type native_handle, int) noexcept {
  if (native_handle->data) {
    detail::state* s = static_cast<detail::state*>(native_handle->data);
		s->invoke_callback();
  }
}

}

#define EV_CONFIG_H <cyan/config.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wextra"
#endif // __GNUC__

#include <cyan/external/ev/ev.c>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif // __GNUC__

