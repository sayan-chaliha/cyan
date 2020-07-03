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

#include <atomic>
#include <memory>
#include <type_traits>

#include <cyan/utility.h>
#include <cyan/noncopyable.h>

namespace cyan::lockfree {

template<typename T, typename Alloc = std::allocator<T>>
class stack : public cyan::noncopyable {
private:
  struct alignas(std::hardware_destructive_interference_size) node {
    node() : next{ nullptr } {}
    node(T&& value) : data{ std::forward<T>(value) }, node() {}

    node* next;
    T data;
  };

public:
  using allocator_type = Alloc;
  using value_type = typename allocator_type::value_type;
  using node_type = node;
  using node_allocator_type = typename std::allocator_traits<Alloc>::template rebind_alloc<node_type>;
  using size_type = typename allocator_type::size_type;
  using reference = value_type&;
  using const_reference = value_type const&;

  stack(allocator_type const& alloc = std::allocator<value_type>()) : node_allocator_{ alloc } {
  }

  ~stack() {
    clear();
  }

  allocator_type get_allocator() const {
    return allocator_type(node_allocator_);
  }

  void push(value_type&& value) {
    node_type* n = node_allocator_.allocate(1);
    new (n) node_type{ std::forward<value_type>(value) };
    n->next = head_.load(std::memory_order_relaxed);

    while (head_.compare_exchange_weak(n->next, n, std::memory_order_acq_rel, std::memory_order_relaxed));
  }

  bool try_pop(reference value) {
    auto head = head_.load(std::memory_order_relaxed);
    while (head_.compare_exchange_weak(head, head->next, std::memory_order_acq_rel, std::memory_order_relaxed));

    if (!head) return false;

    value = std::move(head->data);
    head->~node_type();
    node_allocator_.deallocate(head, 1);

    return true;
  }

  bool empty() const {
    return head_.load(std::memory_order_acquire) == nullptr;
  }

  void clear() {
    auto head = head_.load(std::memory_order_relaxed);
    while (head_.compare_exchange_weak(head, nullptr, std::memory_order_acq_rel, std::memory_order_relaxed));

    while (head) {
      auto n = head;
      head = head->next.load(std::memory_order_relaxed);
      n->~node_type();
      node_allocator_.deallocate(n, 1);
    }
  }

private:
  std::atomic<node_type*> head_;
  node_allocator_type node_allocator_;
};

}
