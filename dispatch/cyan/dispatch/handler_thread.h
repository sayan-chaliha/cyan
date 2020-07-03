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
#include <future>

#include <cyan/noncopyable.h>
#include <cyan/lockfree/queue.h>
#include <cyan/dispatch/handler.h>
#include <cyan/dispatch/message.h>

namespace cyan::dispatch {

class handler_thread : public cyan::noncopyable {
public:
  handler_thread();
  handler_thread(handler& h);
  explicit handler_thread(handler_thread&& other);
  ~handler_thread();

  handler_thread& operator =(handler_thread&& other);

  void stop(bool safe = true);
  bool is_stopping() const;
  bool is_running() const;

  template<typename T>
  void send(T&& payload) {
    enqueue(detail::make_payload_message(std::forward<T>(payload)));
  }

  template<typename T, typename R, typename D>
  void send(T&& payload, std::chrono::duration<R, D> const& timeout) {
    enqueue(detail::make_payload_message(std::forward<T>(payload), timeout));
  }

  template<typename Callable>
  void post(Callable&& callback) {
    enqueue(detail::make_callable_message(std::forward<Callable>(callback)));
  }

  template<typename Callable>
  void post(serial_token const& token, Callable&& callback) {
    auto msg = detail::make_callable_message(std::forward<Callable>(callback));
    msg->set_serial_token(token);
    enqueue(std::move(msg));
  }

  template<typename Callable, typename R, typename D>
  void post(Callable&& callback, std::chrono::duration<R, D> const& timeout) {
    enqueue(detail::make_callable_message(std::forward<Callable>(callback), timeout));
  }

  template<typename Callable>
  [[nodiscard]]
  auto post_awaitable(Callable&& callback) -> std::future<std::invoke_result_t<Callable>> {
    auto task = std::packaged_task<std::invoke_result_t<Callable>()>{ std::forward<Callable>(callback) };
    auto future = task.get_future();

    post(std::move(task));

    return future;
  }

  template<typename Callable>
  [[nodiscard]]
  auto post_awaitable(serial_token const& token, Callable&& callback) -> std::future<std::invoke_result_t<Callable>> {
    auto task = std::packaged_task<std::invoke_result_t<Callable>()>{ std::forward<Callable>(callback) };
    auto future = task.get_future();

    post(token, std::move(task));

    return future;
  }


  template<typename Callable, typename R, typename D>
  [[nodiscard]]
  auto post_awaitable(Callable&& callback, std::chrono::duration<R, D> const& timeout)
        -> std::future<std::invoke_result_t<Callable>> {
    auto task = std::packaged_task<std::invoke_result_t<Callable>()>{ std::forward<Callable>(callback) };
    auto future = task.get_future();

    post(std::move(task), timeout);

    return future;
  }

protected:
  void enqueue(std::unique_ptr<detail::message>&& msg);

private:
  template<
    template<typename, typename> typename QueueType,
    template<typename> typename Alloc = std::allocator
  >
  class handler_thread_impl;

  using default_impl_type = handler_thread_impl<cyan::lockfree::queue>;
  std::unique_ptr<default_impl_type> impl_;
};

}
