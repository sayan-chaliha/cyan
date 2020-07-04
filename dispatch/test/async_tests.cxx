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
#include <gtest/gtest.h>

#include <cyan/dispatch/async.h>
using namespace std::chrono_literals;

class async_tests : public ::testing::Test {
public:
  void SetUp() {
    // Make sure thread pool is started
    cyan::dispatch::global_concurrent_pool();
  }

  void TearDown() {
  }
};

TEST_F(async_tests, async) {
  auto task = std::packaged_task<void()>([] {});
  auto future = task.get_future();

  cyan::dispatch::async(task);
  EXPECT_EQ(future.wait_for(1ms), std::future_status::ready);
}

TEST_F(async_tests, async_timeout) {
  using clock = std::chrono::steady_clock;

  constexpr std::chrono::milliseconds timeout = 100ms;
  auto task = std::packaged_task<void()>([] {});
  auto future = task.get_future();
  
  auto begin = clock::now();
  cyan::dispatch::async(task, timeout);
  future.get();
  auto end = clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  EXPECT_GE(duration, timeout) << "expected " << timeout.count() << "ms got " << duration.count() << "ms";
}

TEST_F(async_tests, async_awaitable) {
  auto future = cyan::dispatch::async_awaitable([] {});
  EXPECT_EQ(future.wait_for(1ms), std::future_status::ready);
}

TEST_F(async_tests, async_serial) {
  cyan::dispatch::serial_token token;
  std::vector<std::int32_t> const expected = { 1, 2, 3, 4, 5 };
  std::vector<std::int32_t> got;

  for (auto& i : expected) {
    cyan::dispatch::async(token, [i, &got] {
      got.push_back(i);
    });
  }

  // Ensures all other tasks are complete
  cyan::dispatch::async_awaitable(token, [] {}).get();

  EXPECT_EQ(got, expected);
}
