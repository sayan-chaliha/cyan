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
#include <mutex>
#include <algorithm>

#include <cyan/dispatch/thread_pool.h>

namespace cyan::dispatch {

std::shared_ptr<thread_pool> global_concurrent_pool() {
  static std::once_flag once_flag;
  static std::shared_ptr<thread_pool> concurrent_pool;

  std::call_once(once_flag, [&] {
    concurrent_pool = std::make_shared<thread_pool>();
  });

  return concurrent_pool;
}

thread_pool::thread_pool(std::uint32_t size) : size_{ std::max(size, 1u) } {
  for (std::uint32_t i = 0; i < size_; i++) {
    threads_.emplace_back(std::make_unique<cyan::dispatch::handler_thread>());
  }
}

thread_pool::~thread_pool() {
  stop();
}

void thread_pool::stop() {
  for (auto& thd : threads_) thd->stop(true);
  threads_.clear();
}

std::uint32_t thread_pool::size() const {
  return size_;
}

std::uint32_t thread_pool::get_next_thread_idx() const {
  auto idx = cur_idx_.load(std::memory_order_relaxed);
  while (cur_idx_.compare_exchange_weak(idx, (idx + 1) % size_));
  return idx;
}

} // cyan::dispatch
