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
#include <cyan/lockfree/freelist.h>

namespace cyan::lockfree {

template<typename T, typename Alloc = std::allocator<T>>
class stack : public cyan::noncopyable {
private:
  static_assert(std::is_move_constructible<T>::value);
  static_assert(std::is_move_assignable<T>::value);

  struct alignas(std::hardware_destructive_interference_size) node {
    node() : next{ nullptr } {}
    node(T const& value) : next{ nullptr }, data{ value } {}
    node(T&& value) : next{ nullptr }, data{ std::forward<T>(value) } {}

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
  using pool_type = detail::freelist<node_type, node_allocator_type>;

  stack(allocator_type const& alloc = std::allocator<value_type>()) : pool_{ alloc } {
    head_.store(nullptr, std::memory_order_release);
  }

  ~stack() {
    clear();
  }

  allocator_type get_allocator() const {
    return pool_.get_allocator();
  }

  void push(value_type&& value) {
    node_type* n = pool_.allocate();
    new (n) node_type{ std::forward<value_type>(value) };
    n->next = head_.load(std::memory_order_relaxed);

    while (!head_.compare_exchange_weak(n->next, n, std::memory_order_acq_rel, std::memory_order_relaxed));
  }

  void push(const_reference value) {
    node_type* n = pool_.allocate();
    new (n) node_type{ value };
    n->next = head_.load(std::memory_order_relaxed);

    while (!head_.compare_exchange_weak(n->next, n, std::memory_order_acq_rel, std::memory_order_relaxed));
  }

  template<typename ...Args>
  void emplace(Args&&... args) {
    node_type* n = pool_.allocate();
    new (n) node_type{ value_type{ std::forward<Args>(args)... }};
    n->next = head_.load(std::memory_order_relaxed);

    while (!head_.compare_exchange_weak(n->next, n, std::memory_order_acq_rel, std::memory_order_relaxed));
  }

  bool try_pop(reference value) {
    auto head = head_.load(std::memory_order_acquire);
    if (!head) return false;

    while (!head_.compare_exchange_weak(head, head->next, std::memory_order_acq_rel, std::memory_order_relaxed)) {
      if (!head) return false;
    }

    value = std::move(head->data);
    head->~node_type();
    pool_.deallocate(head);

    return true;
  }

  bool empty() const {
    return head_.load(std::memory_order_acquire) == nullptr;
  }

  void clear() {
    auto head = head_.load(std::memory_order_acquire);
    while (!head_.compare_exchange_weak(head, nullptr, std::memory_order_acq_rel, std::memory_order_relaxed));

    while (head) {
      auto n = head;
      head = head->next;
      n->~node_type();
      pool_.deallocate(n);
    }
  }

private:
  alignas(std::hardware_destructive_interference_size)
  std::atomic<node_type*> head_;
  
  alignas(std::hardware_destructive_interference_size)
  pool_type pool_;
};

}
