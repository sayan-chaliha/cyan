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

#include <thread>
#include <memory>
#include <vector>
#include <atomic>

#include <cyan/dispatch/handler.h>
#include <cyan/dispatch/handler_thread.h>

namespace cyan::dispatch {

class thread_pool {
public:
  thread_pool(std::uint32_t size = std::thread::hardware_concurrency());
  ~thread_pool();

  void stop();

  template<typename Callable>
  void post(Callable&& callback) {
    threads_[get_next_thread_idx()]->post(std::forward<Callable>(callback));
  }

  template<typename Callable>
  void post(serial_token const& token, Callable&& callback) {
    threads_[get_next_thread_idx()]->post(token, std::forward<Callable>(callback));
  }

  template<typename Callable, typename R, typename D>
  void post(Callable&& callback, std::chrono::duration<R, D> const& timeout) {
    threads_[get_next_thread_idx()]->post(std::forward<Callable>(callback), timeout);
  }

  template<typename Callable>
  auto post_awaitable(Callable&& callback) -> std::future<std::invoke_result_t<Callable>> {
    return threads_[get_next_thread_idx()]->post_awaitable(std::forward<Callable>(callback));
  }

  template<typename Callable>
  auto post_awaitable(serial_token const& token, Callable&& callback) -> std::future<std::invoke_result_t<Callable>> {
    return threads_[get_next_thread_idx()]->post_awaitable(token, std::forward<Callable>(callback));
  }

  template<typename Callable, typename R, typename D>
  auto post_awaitable(Callable&& callback, std::chrono::duration<R, D> const& timeout)
        -> std::future<std::invoke_result_t<Callable>> {
    return threads_[get_next_thread_idx()]->post_awaitable(std::forward<Callable>(callback), timeout);
  }

  std::uint32_t size() const;

private:
  std::uint32_t get_next_thread_idx() const;

  std::uint32_t const size_;
  std::vector<std::unique_ptr<cyan::dispatch::handler_thread>> threads_;
  mutable std::atomic<std::uint32_t> cur_idx_;
};

std::shared_ptr<thread_pool> global_concurrent_pool();

} // cyan::dispatch
