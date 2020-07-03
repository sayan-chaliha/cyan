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

#include <cyan/utility.h>

namespace cyan::lockfree::detail {

template<typename T, typename Alloc = std::allocator<T>>
class freelist {
private:
  struct alignas(std::hardware_destructive_interference_size) node {
     node* next;
  };

  static_assert(sizeof(node) <= sizeof(T));

public:
  using value_type = T;
  using pointer = value_type*;
  using allocator_type = Alloc;
  using reference = value_type&;
  using const_reference = value_type const&;

  freelist(allocator_type const& alloc = std::allocator<value_type>(), std::size_t max_size = 1000lu)
        : max_size_{ max_size }, size_{ 0 },  head_{ nullptr }, allocator_{ alloc } {
    for (std::size_t i = 0; i < max_size_; i++) {
      deallocate(allocator_.allocate(1));
    }
  }

  ~freelist() {
    node* head = head_.load(std::memory_order_relaxed);
    while (head_.compare_exchange_weak(head, head->next, std::memory_order_relaxed, std::memory_order_relaxed)) {
      if (!head) break;
      allocator_.deallocate(reinterpret_cast<pointer>(head), 1);
    }
  }

  pointer allocate() {
    node* head = head_.load(std::memory_order_consume);
    if (!head) {
      return allocator_.allocate(1);
    }
    while (!head_.compare_exchange_weak(head, head->next, std::memory_order_acq_rel, std::memory_order_relaxed));

    size_--;
    return reinterpret_cast<pointer>(head);
  }

  void deallocate(pointer value) {
    if (size_ == max_size_) {
      allocator_.deallocate(value, 1);
      return;
    }

    node* n = reinterpret_cast<node*>(value);
    n->next = head_.load(std::memory_order_consume);
    while (!head_.compare_exchange_weak(n->next, n, std::memory_order_acq_rel, std::memory_order_relaxed));
    size_++;
  }

  void reserve(std::size_t size) {
    max_size_ = size;
  }

  allocator_type get_allocator() const {
    return allocator_;
  }

  std::size_t max_size() const {
    return max_size_;
  }

  std::size_t size() const {
    return size_;
  }

private:
  std::atomic<std::size_t> const max_size_;
  std::atomic<std::size_t> size_;
  std::atomic<node*> head_;
  allocator_type allocator_;
};

} // cyan::lockfree:detail
