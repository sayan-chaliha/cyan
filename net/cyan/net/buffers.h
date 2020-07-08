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

namespace cyan::net {

class mutable_buffer {
public:
  mutable_buffer() noexcept : data_{ nullptr }, size_{ 0 } {
  }

  mutable_buffer(void* data, std::size_t size) noexcept : data_{ data }, size_{ size } {
  }

  template<typename Container>
  mutable_buffer(Container& container) noexcept : data_{ container.data() },
        size_{ container.capacity() * sizeof(typename Container::value_type) } {
  }

  void* data() noexcept {
    return data_;
  }

  std::size_t size() const noexcept {
    return size_;
  }

private:
  void* data_;
  std::size_t size_;
};

class const_buffer {
public:
  const_buffer() noexcept : data_{ nullptr }, size_{ 0 } {
  }

  const_buffer(void const* data, std::size_t size) noexcept : data_{ data }, size_{ size } {
  }

  template<typename Container>
  const_buffer(Container& container) noexcept : data_{ container.data() },
        size_{ container.capacity() * sizeof(typename Container::value_type) } {
  }

  void const* data() const noexcept {
    return data_;
  }

  std::size_t size() const noexcept {
    return size_;
  }

private:
  void const* data_;
  std::size_t size_;
};

}
