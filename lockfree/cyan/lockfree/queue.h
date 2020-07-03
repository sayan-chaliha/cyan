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
#include <cyan/lockfree/tagged_ptr.h>

namespace cyan::lockfree {

template<
  typename T,
  typename Alloc = std::allocator<T>
>
class queue : public cyan::noncopyable {
private:
  static_assert(std::is_move_constructible<T>::value);
  static_assert(std::is_move_assignable<T>::value);

  struct alignas(std::hardware_destructive_interference_size) node {
    using tagged_handle_type = detail::tagged_ptr<node>;
    using handle_type = node*;
    constexpr static handle_type null_handle = handle_type();

    node() : next{ tagged_handle_type{ null_handle, 0 } } {}
    node(T&& value) : next{ tagged_handle_type{ null_handle, 0 } }, data{ std::forward<T>(value) } {}

    std::atomic<tagged_handle_type> next;
    T data;
  };

public:
  using allocator_type = Alloc;
  using value_type = typename allocator_type::value_type;
  using node_allocator_type = typename std::allocator_traits<Alloc>::template rebind_alloc<node>;
  using pool_type = detail::freelist<node, node_allocator_type>;
  using size_type = typename allocator_type::size_type;
  using reference = value_type&;
  using const_reference = value_type const&;
  using node_type = node;

  queue(allocator_type const& alloc = std::allocator<value_type>())
        : head_{ tagged_node_handle_type{node::null_handle, 0} },
        tail_{ tagged_node_handle_type{node::null_handle, 0} },
        pool_{ alloc } {
    initialize();
  }

  ~queue() {
    clear();
  }

  allocator_type get_allocator() const {
    return allocator_type(pool_.get_allocator());
  }

  template<typename ...Args>
  void emplace(Args&&... args) {
    node* n = pool_.allocate();
    new (n) node{ value_type{ std::forward<Args>(args)... } };
    enqueue(n);
  }

  void enqueue(const_reference value) {
    node* n = pool_.allocate();
    new (n) node{ value };
    enqueue(n);
  }

  void enqueue(value_type&& value) {
    node* n = pool_.allocate();
    new (n) node{ std::forward<value_type>(value) };
    enqueue(n);
  }

  bool try_dequeue(reference value) {
    for (;;) {
      auto head = head_.load(std::memory_order_acquire);
      auto tail = tail_.load(std::memory_order_acquire);
      auto next = head->next.load(std::memory_order_acquire);
      tagged_node_handle_type node;

      if (head == head_.load(std::memory_order_acquire)) {
        if (head.get_ptr() == tail.get_ptr()) {
          if (!next) return false;
          node.set_ptr(next.get_ptr());
          node.set_tag(tail.get_next_tag());
          tail_.compare_exchange_strong(tail, node, std::memory_order_release, std::memory_order_relaxed);
        } else {
          value = std::move(next->data);
          node.set_ptr(next.get_ptr());
          node.set_tag(head.get_next_tag());
          if (head_.compare_exchange_strong(head, node, std::memory_order_release, std::memory_order_relaxed)) {
            head.get_ptr()->~node();
            pool_.deallocate(head.get_ptr());
            return true;
          }
        }
      }
    }
  }

  bool empty() const {
    return tail_.load(std::memory_order_acquire).get_ptr() == head_.load(std::memory_order_acquire).get_ptr();
  }

  bool is_lock_free() const {
    return tail_.is_lock_free() && head_.is_lock_free();
  }

  void reserve(std::size_t size) {
    pool_.reserve(size);
  }

  void clear() {
    auto head = head_.load(std::memory_order_acquire);
    auto tail = tail_.load(std::memory_order_acquire);

    while (head.get_ptr() != tail.get_ptr()) {
      node* n = head.get_ptr();
      head = head->next.load(std::memory_order_acquire);
      n->~node();
      pool_.deallocate(n);
    }
    tail->data.~value_type();

    initialize();
  }

private:
  using tagged_node_handle_type = typename node::tagged_handle_type;

  void initialize() {
    node* n = pool_.allocate();
    new (n) node{};
    tagged_node_handle_type dummy{ n, 0 };
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_release);
  }

  void enqueue(node* n) {
    tagged_node_handle_type new_tail{ n, 0 };

    for (;;) {
      auto tail = tail_.load(std::memory_order_acquire);
      auto next = tail->next.load(std::memory_order_acquire);

      if (tail == tail_.load(std::memory_order_acquire)) {
        if (!next) {
          new_tail.set_ptr(n);
          new_tail.set_tag(next.get_next_tag());
          if (tail->next.compare_exchange_strong(next, new_tail,
                std::memory_order_release, std::memory_order_relaxed)) {
            new_tail.set_ptr(n);
            new_tail.set_tag(tail_.load().get_next_tag());
            tail_.compare_exchange_strong(tail, new_tail,
                std::memory_order_release, std::memory_order_relaxed);
            return;
          }
        } else {
          new_tail.set_ptr(next.get_ptr());
          new_tail.set_tag(tail.get_next_tag());
          tail_.compare_exchange_strong(tail, new_tail,
                std::memory_order_release, std::memory_order_relaxed);
        }
      }
    }
  }

  alignas(std::hardware_destructive_interference_size)
  std::atomic<tagged_node_handle_type> head_;

  alignas(std::hardware_destructive_interference_size)
  std::atomic<tagged_node_handle_type> tail_;

  alignas(std::hardware_destructive_interference_size)
  pool_type pool_;
};

} // cyan::lockfree
