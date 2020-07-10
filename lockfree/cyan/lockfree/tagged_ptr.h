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

#include <cstddef>
#include <limits>

namespace cyan::lockfree::detail {

template<typename T>
// TODO: calculate alingment requirements
class tagged_ptr {
public:
  using tag_type = std::size_t;
  using pointer_type = T*;
  using reference_type = T&;
  using const_pointer_type = T const*;
  using const_reference_type = T const&;

  tagged_ptr() noexcept : ptr_{ nullptr }, tag_{ 0 } {}
  explicit tagged_ptr(pointer_type ptr, tag_type tag = 0) noexcept : ptr_{ ptr }, tag_{ tag } {}
  tagged_ptr(tagged_ptr const&) noexcept = default;
  tagged_ptr(tagged_ptr&&) noexcept = default;

  tagged_ptr& operator =(tagged_ptr const&) noexcept = default;

  bool operator ==(tagged_ptr const& other) const noexcept {
    return ptr_ == other.ptr_ && tag_ == other.tag_;
  }

  bool operator !=(tagged_ptr const& other) const noexcept {
    return !operator ==(other);
  }

  tag_type get_tag() const noexcept {
    return tag_;
  }

  void set_tag(tag_type t) noexcept {
    tag_ = t;
  }

  pointer_type get_ptr() const noexcept {
    return ptr_;
  }

  void set_ptr(pointer_type ptr) noexcept {
    ptr_ = ptr;
  }

  tag_type get_next_tag() const noexcept {
    return (tag_ + 1) & std::numeric_limits<tag_type>::max();
  }

  const_pointer_type operator ->() const noexcept {
    return ptr_;
  }

  pointer_type operator ->() noexcept {
    return ptr_;
  }

  const_reference_type operator *() const noexcept {
    return *ptr_;
  }

  reference_type operator *() noexcept {
    return *ptr_;
  }

  operator bool() const noexcept {
    return ptr_ != nullptr;
  }

private:
  pointer_type ptr_;
  tag_type tag_;
};

} // cyan::lockfree::detail
