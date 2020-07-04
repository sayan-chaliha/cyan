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

#include <thread>
#include <future>

#include <cyan/event.h>
using namespace std::chrono_literals;

// Instead of mocking the backend traits, these
// tests involve integration testing with the actual
// event backend, and behavior of the various event
// types.
class event_tests : public ::testing::Test {
public:
  void SetUp() {
  }

  void TearDown() {
  }
};

TEST_F(event_tests, loop_construction) {
  auto main_loop = cyan::event::get_main_loop();
  EXPECT_NE(main_loop, nullptr);

  std::thread thread{[main_loop = cyan::this_thread::get_event_loop()] {
    auto loop = cyan::this_thread::get_event_loop();

    EXPECT_NE(loop, nullptr);
    EXPECT_NE(loop, main_loop);
  }};

  thread.join();
}

TEST_F(event_tests, async) {
  std::promise<std::shared_ptr<cyan::event::async>> async_promise;
  std::promise<void> exit_promise;

  std::thread thread{[&async_promise, &exit_promise] {
    auto async = std::make_shared<cyan::event::async>(cyan::this_thread::get_event_loop());
    async->set_callback([] {
      cyan::this_thread::get_event_loop()->stop();
    });

    EXPECT_FALSE(async->is_active());
    async->start();
    EXPECT_TRUE(async->is_active());

    async_promise.set_value(async);

    cyan::this_thread::get_event_loop()->start();

    async->stop();
    exit_promise.set_value();
  }};
  thread.detach();

  auto async = async_promise.get_future().get();
  async->send();

  EXPECT_TRUE(async->is_pending());
  EXPECT_EQ(exit_promise.get_future().wait_for(5ms), std::future_status::ready);
}

TEST_F(event_tests, timer) {
  using clock = std::chrono::steady_clock;

  constexpr std::chrono::milliseconds timeout{ 100ms };
  clock::time_point begin, end;

  cyan::event::timer timer{ cyan::this_thread::get_event_loop() };
  timer.set_timeout(timeout);
  timer.set_callback([&end] {
    end = clock::now();
    cyan::this_thread::get_event_loop()->stop();
  });

  EXPECT_FALSE(timer.is_active());
  timer.start();
  EXPECT_TRUE(timer.is_active());

  begin = clock::now();
  cyan::this_thread::get_event_loop()->start();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
  EXPECT_GE(duration, timeout) << "expected " << timeout.count() << "ms; got " << duration.count() << "ms";
}

TEST_F(event_tests, timer_wheel) {
  cyan::event::timer_wheel timer_wheel{ cyan::this_thread::get_event_loop() };

  EXPECT_FALSE(timer_wheel.is_active());
  timer_wheel.start();
  // Timer isn't really started till there's as scheduled task
  EXPECT_FALSE(timer_wheel.is_active());

  auto task1 = std::packaged_task<void()>([] {});

  EXPECT_FALSE(timer_wheel.is_pending());
  auto task1_id = timer_wheel.post(task1, cyan::event::timer_wheel::default_resolution + 5ms);
  EXPECT_TRUE(timer_wheel.is_active());
  EXPECT_TRUE(timer_wheel.is_pending());

  auto task2 = std::packaged_task<void()>([&task1_id, &timer_wheel] {
    timer_wheel.cancel(task1_id);
    cyan::this_thread::get_event_loop()->stop();
  });
  timer_wheel.post(task2, cyan::event::timer_wheel::default_resolution);

  cyan::this_thread::get_event_loop()->start();
  EXPECT_EQ(task1.get_future().wait_for(0ms), std::future_status::timeout);
  EXPECT_EQ(task2.get_future().wait_for(0ms), std::future_status::ready);
}

TEST_F(event_tests, signal) {
  cyan::event::signal signal{ cyan::event::get_main_loop() };
  auto promise = std::promise<void>{};

  signal.set_callback([&promise] {
    cyan::event::get_main_loop()->stop();
    promise.set_value();
  });

  EXPECT_FALSE(signal.is_active());
  signal.start();
  EXPECT_TRUE(signal.is_active());

  std::thread thread{[] {
    raise(SIGINT);
  }};
  thread.detach();

  cyan::event::get_main_loop()->start();
  ASSERT_EQ(promise.get_future().wait_for(0ms), std::future_status::ready);
}

TEST_F(event_tests, idle) {
  // Tests that idle executes, but not really certain
  // that the loop is actually idle at that point
  cyan::event::idle idle{ cyan::event::get_main_loop() };
  auto promise = std::promise<void>{};

  idle.set_callback([&promise] {
    cyan::event::get_main_loop()->stop();
    promise.set_value();
  });

  EXPECT_FALSE(idle.is_active());
  idle.start();
  EXPECT_TRUE(idle.is_active());

  cyan::event::get_main_loop()->start();
  ASSERT_EQ(promise.get_future().wait_for(0ms), std::future_status::ready);
}
