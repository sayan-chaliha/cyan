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

#include <cstdint>
#include <array>

namespace cyan::uuids {

class uuid {
private:
  constexpr static std::size_t static_size = 16;
  using rep_type = std::array<std::uint8_t, static_size>;

public:
  enum class variant_type {
    unknown,
    rfc_4122
  };

  enum class version_type {
    unknown,
    random_number_based
  };

  using value_type = typename rep_type::value_type;
  using reference = typename rep_type::reference;
  using const_reference = typename rep_type::const_reference;
  using iterator = typename rep_type::iterator;
  using const_iterator = typename rep_type::const_iterator;
  using reverse_iterator = typename rep_type::reverse_iterator;
  using const_reverse_iterator = typename rep_type::const_reverse_iterator;
  using size_type = typename rep_type::size_type;
  using difference_type = typename rep_type::difference_type;

  iterator begin() { return data_.begin(); }
  const_iterator cbegin() const { return data_.cbegin(); }
  iterator end() { return data_.end(); }
  const_iterator end() const { return data_.cend(); }
  reverse_iterator rbegin() { return data_.rbegin(); }
  const_reverse_iterator crbegin() { return data_.crbegin(); }
  reverse_iterator rend() { return data_.rend(); }
  const_reverse_iterator crend() { return data_.crend(); }
  constexpr size_type size() const { return static_size; }

  variant_type variant() const {
    const_reference octet7 = data[8];
  }

  version_type version() const {
  }

private:
  rep_type data_;
};

} // cyan::uuids
