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

#include <cyan/dispatch/thread_pool.h>

namespace cyan::dispatch {

template<typename Callable>
void async(Callable&& callable) {
  global_concurrent_pool()->post(std::forward<Callable>(callable));
}

template<typename Callable>
void async(serial_token const& token, Callable&& callable) {
  global_concurrent_pool()->post(token, std::forward<Callable>(callable));
}

template<typename Callable, typename R, typename D>
void async(Callable&& callable, std::chrono::duration<R, D> const& timeout) {
  global_concurrent_pool()->post(std::forward<Callable>(callable), timeout);
}

template<typename Callable>
[[nodiscard]]
auto async_awaitable(Callable&& callable) -> std::future<std::invoke_result_t<Callable>> {
  return global_concurrent_pool()->post_awaitable(std::forward<Callable>(callable));
}

template<typename Callable>
[[nodiscard]]
auto async_awaitable(serial_token const& token, Callable&& callable) -> std::future<std::invoke_result_t<Callable>> {
  return global_concurrent_pool()->post_awaitable(token, std::forward<Callable>(callable));
}

template<typename Callable, typename R, typename D>
[[nodiscard]]
auto async_awaitable(Callable&& callable, std::chrono::duration<R, D> const& timeout)
      -> std::future<std::invoke_result_t<Callable>> {
  return global_concurrent_pool()->post_awaitable(std::forward<Callable>(callable), timeout);
}

} // cyan::dispatch
