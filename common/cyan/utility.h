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

#include <tuple>

#ifndef HAVE_HARDWARE_DESTRUCTIVE_INTERFERENCE_SIZE
namespace std {

constexpr size_t hardware_destructive_interference_size = 
#if defined(__x86_64__) || defined(_M_X64)
  64;
#elif defined(__i386__) || defined(_M_I86)
  32;
#elif defined(__arm__) || defined(_M_ARM)
  128;
#endif // __x86_64__

}
#endif // HAVE_HARDWARE_DESTRUCTIVE_INTERFERENCE_SIZE

namespace cyan {

namespace detail {

template<typename ...Args, std::size_t ...Is>
constexpr auto tuple_remove_last_helper(std::tuple<Args...> tuple, std::index_sequence<Is...>) {
  return std::make_tuple(std::get<Is>(tuple)...);
}

}

template<typename ...Args>
constexpr auto tuple_remove_last(std::tuple<Args...> const& tuple) {
  static_assert(std::tuple_size<std::tuple<Args...>>::value >= 1);
  return detail::tuple_remove_last_helper(tuple, std::make_index_sequence<sizeof ...(Args) - 1>());
}

} // cyan
