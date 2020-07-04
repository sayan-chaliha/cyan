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

#include <cyan/lockfree/queue.h>

class queue_test : public ::testing::Test {
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

TEST_F(queue_test, enqueue_and_dequeue) {
  std::int32_t i = 1;
  cyan::lockfree::queue<std::int32_t> queue;

  queue.enqueue(i);

  i = 2134;
  ASSERT_FALSE(queue.empty()) << "queue cannot be empty after enqueuing";
  ASSERT_TRUE(queue.try_dequeue(i)) << "dequeue should not have failed";
  ASSERT_EQ(i, 1) << "i should be equal to 1";
  ASSERT_TRUE(queue.empty()) << "queue should be empty";
}

TEST_F(queue_test, emplace) {
  dummy value;
  cyan::lockfree::queue<dummy> queue;

  queue.emplace(1);

  ASSERT_FALSE(queue.empty()) << "queue cannot be empty after enqueuing";
  ASSERT_TRUE(queue.try_dequeue(value)) << "dequeue should not have failed";
  ASSERT_EQ(value.data, 1) << "value.data should be equal to 1";
  ASSERT_TRUE(queue.empty()) << "queue should be empty";
}

TEST_F(queue_test, owning_data) {
  std::unique_ptr<std::int32_t> num = std::make_unique<std::int32_t>(1);
  cyan::lockfree::queue<std::unique_ptr<std::int32_t>> queue;

  queue.enqueue(std::move(num));

  ASSERT_FALSE(queue.empty()) << "queue cannot be empty after enqueuing";
  ASSERT_TRUE(queue.try_dequeue(num)) << "dequeue should not have failed";
  ASSERT_EQ(*num, 1) << "value.data should be equal to 1";
  ASSERT_TRUE(queue.empty()) << "queue should be empty";
}

TEST_F(queue_test, multithread_produce) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::queue<std::int32_t> queue;

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&queue, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) queue.enqueue(i);
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the queue should have been successfully
  // pumped simultaneously from multiple threads
  ASSERT_FALSE(queue.empty());

  for (auto i = 0; i < thread_count * item_count; i++) {
    std::int32_t value;
    EXPECT_TRUE(queue.try_dequeue(value)) << "queue state inconsistent; item missing";
  }
}

TEST_F(queue_test, multithread_consume) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::queue<std::int32_t> queue;

  for (auto i = 0; i < thread_count * item_count; i++) {
    queue.enqueue(i);
  }

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&queue, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        std::int32_t value;
        EXPECT_TRUE(queue.try_dequeue(value)) << "dequeue should not fail";
      }
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the queue should have been successfully
  // drained simultaneously from multiple threads
  ASSERT_TRUE(queue.empty());
}

TEST_F(queue_test, multithread_produce_consume) {
  constexpr std::int32_t thread_count = 10;
  constexpr std::int32_t item_count = 10;
  std::atomic<bool> go = false;
  std::vector<std::thread> threads;
  cyan::lockfree::queue<std::int32_t> queue;

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&queue, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        queue.enqueue(i);
      }
    });
  }

  for (auto i = 0; i < thread_count; i++) {
    threads.emplace_back([&queue, &go] {
      while (!go);
      for (auto i = 0; i < item_count; i++) {
        std::int32_t value;
        if (!queue.try_dequeue(value)) {
          // If a dequeue fails, the producer hasn't
          // produced an element yet. Retry.
          i--;
        }
      }
    });
  }

  // make sure all threads are started
  threads.emplace_back([&go] { go = true; });

  for (auto& thread : threads) thread.join();

  // At this point, the queue should have been successfully
  // pumped and drained simultaneously from multiple threads
  ASSERT_TRUE(queue.empty());
}
