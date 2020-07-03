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

#include <random>

#include <cyan/noncopyable.h>
#include <cyan/uuid/uuid.h>

namespace cyan::uuids {

template<typename T = std::mt19937>
class random_generator : public cyan::noncopyable {
public:
  using result_type = uuid;
  using random_engine_type = T;
  using distribution_type = std::uniform_int_distrbution<unsigned long>;

  random_generator() : random_engine_{ std::random_device() } {
  }

  random_generator(random_engine_type const& engine) : random_engine_{ engine } {
  }

  random_generator(random_generator&& other) : random_engine_{ std::move(other.random_engine_) } {
  }


private:
  static result_type& set_version_info(result_type& u) {
    // variant; must be 0b10xxxxxx
    *(u.begin() + 8) &= 0xBF; // 0b10111111
    *(u.begin() + 8) |= 0x80; // 0b10010000

    // version; must be 0b0100xxxx
    *(u.begin() + 6) &= 0x4F; // 0b01001111
    *(u.begin() + 6) |= 0x40; // 0b01000000

    return u;
  }

  distribution_type distribution_;
  random_engine_type random_engine_;
};

} // cyan::uuids
