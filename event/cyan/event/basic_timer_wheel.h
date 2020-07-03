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

#include <map>
#include <chrono>
#include <memory>
#include <utility>
#include <unordered_map>
#include <functional>

#include <cyan/utility.h>
#include <cyan/noncopyable.h>
#include <cyan/event/basic_loop.h>
#include <cyan/event/basic_timer.h>

namespace cyan::event {

template<typename T>
class basic_timer_wheel : public cyan::noncopyable {
public:
  using backend_traits_type = T;
  using request_id_type = std::size_t;
  using timer_type = basic_timer<backend_traits_type>;
  constexpr static auto default_resolution = std::chrono::milliseconds(100);

private:
	struct request {
    virtual ~request() = default;

    request_id_type id;
    bool is_cancelled;
    std::chrono::milliseconds timeout;
    std::chrono::steady_clock::time_point expiry;
    virtual void process() = 0;
	};

  template<typename C>
  struct request_impl : public request {
    request_impl(C&& callable) : callable_{ std::forward<C>(callable) } {}

    void process() override {
      callable_();
    }

  private:
    C callable_;
  };

  template<typename Callable>
  std::shared_ptr<request> make_request(Callable&& c) {
    return std::shared_ptr<request>{ new request_impl<Callable>{ std::forward<Callable>(c) }};
  }

  struct basic_timer_wheel_impl {
    basic_timer_wheel_impl(std::weak_ptr<basic_loop<backend_traits_type>> const& loop_ref) : timer{ loop_ref },
          request_id_{ 0 } {
    }

    request_id_type get_next_request_id() {
      auto id = request_id_;
      request_id_ = (request_id_ + 1) & std::numeric_limits<request_id_type>::max();
      return id;
    }

    template<typename R, typename D>
    static std::chrono::steady_clock::time_point get_absolute_expiry(std::chrono::duration<R, D> const& timeout) {
      auto const now = std::chrono::steady_clock::now();
      return now + timeout;
    }

    std::unordered_map<request_id_type, std::shared_ptr<request>> requests;
    std::multimap<std::chrono::steady_clock::time_point, std::shared_ptr<request>> time_hiearchy;
    timer_type timer;

  private:
    request_id_type request_id_;
  };

public:
  basic_timer_wheel(std::weak_ptr<basic_loop<backend_traits_type>> const& loop_ref)
        : impl_{ std::make_unique<basic_timer_wheel_impl>(loop_ref) } {
    impl_->timer.set_timeout(default_resolution);
    impl_->timer.set_callback([this] { bookkeeper(); });
  }

  template<typename R, typename D>
  basic_timer_wheel(std::weak_ptr<basic_loop<backend_traits_type>> const& loop_ref,
        std::chrono::duration<R, D> const& res)
        : basic_timer_wheel(loop_ref) {
    impl_->timer.set_timeout(res);
  }

  explicit basic_timer_wheel(basic_timer_wheel&& other) : impl_{ std::move(other.impl_) } {
    other.impl_ = nullptr;
  }

  ~basic_timer_wheel() {
    if (impl_) stop();
  }

  void start() {
    if (impl_->requests.size() && !impl_->timer.is_active()) impl_->timer.start();
  }

  void stop() {
		impl_->timer.stop();
  }

  template<typename Callable, typename R, typename D>
  request_id_type post(Callable&& cb, std::chrono::duration<R, D> const& timeout) {
    auto req = make_request(std::forward<Callable>(cb));
    req->id = impl_->get_next_request_id();
    req->is_cancelled = false;
    req->timeout = std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
    req->expiry = impl_->get_absolute_expiry(timeout);

    impl_->requests.emplace(std::make_pair(req->id, req));
    impl_->time_hiearchy.emplace(std::make_pair(req->expiry, req));

    start();

    return req->id;
  }

  void cancel(request_id_type request_id) {
    auto it = impl_->requests.find(request_id);
    if (it != impl_->requests.end()) {
      it->second->is_cancelled = true;
    }
  }

  void reset(request_id_type request_id) {
    auto it = impl_->requests.find(request_id);
    if (it != impl_->requests.end()) {
      it->second->expiry = impl_->get_absolute_expiry(it->second->timeout);
    }
  }

  bool is_active() const {
    return impl_->timer.is_active();
  }

  bool is_pending() const {
    return !impl_->requests.empty();
  }

private:
  void bookkeeper() noexcept {
    auto const now = std::chrono::steady_clock::now();

    for (auto i = impl_->time_hiearchy.begin(); i != impl_->time_hiearchy.end();) {
      auto& entry = *i;
      if (entry.second->expiry != entry.first) {
        auto rep = entry.second;
        i = impl_->time_hiearchy.erase(i);
        i = impl_->time_hiearchy.emplace(std::make_pair(rep->expiry, rep));
      } else if ((entry.first - now).count() <= 0) {
        if (!entry.second->is_cancelled) {
          entry.second->process();
        }

        impl_->requests.erase(entry.second->id);
        i = impl_->time_hiearchy.erase(i);
      } else {
        break;
      }
    }

    if (impl_->time_hiearchy.empty()) stop();
  }

  std::unique_ptr<basic_timer_wheel_impl> impl_;
};

} // cyan::event
