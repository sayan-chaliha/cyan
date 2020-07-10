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
#include <stdexcept>

#include <cyan/event/basic_loop.h>
#include <cyan/noncopyable.h>

namespace cyan::event {
inline namespace v1 {

template<typename BackendTraits, template<typename> typename Event>
class event_base : public cyan::noncopyable {
private:
  using event_type = Event<BackendTraits>;

public:
  using loop_type = basic_loop<BackendTraits>;

  event_base(std::weak_ptr<loop_type> const& loop) noexcept : loop_ref_{ loop } {
  }

  explicit event_base(event_base&& other) noexcept : loop_ref_{ std::move(other.loop_ref_) } {
  }

  event_base& operator =(event_base&& other) noexcept {
    loop_ref_ = std::move(other.loop_ref_);
    return *this;
  }

  ~event_base() = default;

  void start() {
    using event_traits_type = typename event_type::backend_traits_type;

    if (auto loop = loop_ref_.lock()) {
      event_traits_type::start(loop->native_handle(), static_cast<event_type*>(this)->native_handle());
    } else {
      throw std::runtime_error{ "loop is dead" };
    }
  }

  void stop() noexcept {
    using event_traits_type = typename event_type::backend_traits_type;

    if (auto loop = loop_ref_.lock()) {
      event_traits_type::stop(loop->native_handle(), static_cast<event_type*>(this)->native_handle());
    }
  }

  bool is_pending() const noexcept {
    using event_traits_type = typename event_type::backend_traits_type;

    return event_traits_type::is_pending(static_cast<event_type const*>(this)->native_handle());
  }

  bool is_active() const noexcept {
    using event_traits_type = typename event_type::backend_traits_type;

    return event_traits_type::is_active(static_cast<event_type const*>(this)->native_handle());
  }

  void set_loop(std::weak_ptr<loop_type> const& loop) noexcept {
    bool was_active = is_active();
    if (was_active) {
      stop();
    }

    loop_ref_ = loop;

    if (was_active) {
      start();
    }
  }

protected:
  std::weak_ptr<loop_type> loop_ref_;
};

} // v1
} // cyan::event
