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

#include <atomic>
#include <thread>
#include <vector>

#include <cyan/lockfree/stack.h>

class stack_test : public ::testing::Test {
public:
  void SetUp() {
  }

  void TearDown() {
  }

protected:
};

struct dummy : cyan::noncopyable {
  dummy() : data{ 0 } {}
  dummy(std::int32_t i) : data{ i } {}

  dummy(dummy&& other) : data{ std::move(other.data) } {}
  dummy& operator =(dummy&& other) { data = other.data; return *this; }

  std::int32_t data;
};

TEST_F(stack_test, push_and_pop) {
  std::int32_t i = 1;
  cyan::lockfree::stack<std::int32_t> stack;

  stack.push(i);

  i = 2134;
  ASSERT_FALSE(stack.empty()) << "stack cannot be empty after enqueuing";
  ASSERT_TRUE(stack.try_pop(i)) << "destack should not have failed";
  ASSERT_EQ(i, 1) << "i should be equal to 1";
  ASSERT_TRUE(stack.empty()) << "stack should be empty";
}

TEST_F(stack_test, emplace) {
  dummy value;
  cyan::lockfree::stack<dummy> stack;

  stack.emplace(1);

  ASSERT_FALSE(stack.empty()) << "stack cannot be empty after enqueuing";
  ASSERT_TRUE(stack.try_pop(value)) << "destack should not have failed";
  ASSERT_EQ(value.data, 1) << "value.data should be equal to 1";
  ASSERT_TRUE(stack.empty()) << "stack should be empty";
}

TEST_F(stack_test, owning_data) {
  std::unique_ptr<std::int32_t> num = std::make_unique<std::int32_t>(1);
  cyan::lockfree::stack<std::unique_ptr<std::int32_t>> stack;

  stack.push(std::move(num));

  ASSERT_FALSE(stack.empty()) << "stack cannot be empty after enqueuing";
  ASSERT_TRUE(stack.try_pop(num)) << "destack should not have failed";
  ASSERT_EQ(*num, 1) << "value.data should be equal to 1";
  ASSERT_TRUE(stack.empty()) << "stack should be empty";
}

TEST_F(stack_test, multithread_produce) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::stack<std::int32_t> stack;

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&stack, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) stack.push(i);
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the stack should have been successfully
  // pumped simultaneously from multiple threads
  ASSERT_FALSE(stack.empty());

  for (auto i = 0; i < thread_count * item_count; i++) {
    std::int32_t value;
    ASSERT_TRUE(stack.try_pop(value)) << "stack state inconsistent; item missing";
  }
}

TEST_F(stack_test, multithread_consume) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::stack<std::int32_t> stack;

  for (auto i = 0; i < thread_count * item_count; i++) {
    stack.push(i);
  }

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&stack, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        std::int32_t value;
        EXPECT_TRUE(stack.try_pop(value)) << "destack should not fail";
      }
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the stack should have been successfully
  // drained simultaneously from multiple threads
  ASSERT_TRUE(stack.empty());
}

TEST_F(stack_test, multithread_produce_consume) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::stack<std::int32_t> stack;

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&stack, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        stack.push(i);
      }
    });
  }

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&stack, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        std::int32_t value;
        if (!stack.try_pop(value)) {
          // If a destack fails, the producer hasn't
          // produced an element yet. Retry.
          i--;
        }
      }
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the stack should have been successfully
  // pumped and drained simultaneously from multiple threads
  ASSERT_TRUE(stack.empty());
}
