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
#include <functional>

namespace cyan {

namespace detail {

template<typename T>
struct callable {
  using tuple_type = T;
  using indices_type = std::make_index_sequence<std::tuple_size<tuple_type>::value>;

  template<std::size_t Idx>
  static std::tuple_element_t<Idx, tuple_type>&& decl_val();

  template<std::size_t ...Idx, typename ...Args>
  auto invoke(std::index_sequence<Idx...>, Args&&... args)
        noexcept(noexcept(std::invoke(decl_val<Idx>()..., std::forward<Args>(args)...)))
        -> decltype(std::invoke(decl_val<Idx>()..., std::forward<Args>(args)...)) {
    return std::invoke(std::get<Idx>(std::move(tuple))..., std::forward<Args>(args)...);
  }

  template<typename ...Args>
  auto operator ()(Args&&... args)
        noexcept(noexcept(std::declval<callable&>().invoke(indices_type(), std::forward<Args>(args)...)))
        -> decltype(std::declval<callable&>().invoke(indices_type(), std::forward<Args>(args)...)) {
    return invoke(indices_type(), std::forward<Args>(args)...);
  }

  tuple_type tuple;
};

} // detail


template<typename ...T>
using callable = detail::callable<decltype(std::make_tuple(std::declval<T>()...))>;

template<typename F, typename ...Args>
callable<F, Args...> make_callable(F&& f, Args&&... args) {
  // static_assert(std::is_invocable_v<F, Args...>, "make_callable: type is not invocable");
  return {{ std::make_tuple(std::forward<F>(f), std::forward<Args>(args)...) }};
}

} // cyan
