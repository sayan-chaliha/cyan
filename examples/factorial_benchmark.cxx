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
#include <algorithm>
#include <iostream>
#include <vector>

#include <cyan/dispatch.h>

// Factorial function
std::size_t factorial(std::size_t i) {
  i = std::max(1ul, i);

  std::size_t r = i;
  while (i > 2) {
    r *= --i;
  }

  return r;
}

int main(int, char**) {
  constexpr std::size_t iterations = 300000ul;
  using clock = std::chrono::high_resolution_clock;
  clock::time_point begin, end;

  std::vector<std::future<std::size_t>> futures;
  std::vector<std::size_t> results;

  futures.reserve(iterations);
  results.reserve(iterations);

  std::cout << "--- test 1: using std::async ---" << std::endl;
  begin = clock::now();
  for (std::size_t i = 0; i < iterations; i++) {
    futures[i] = std::async([i] { return factorial(i); });
  }
  for (std::size_t i = 0; i < iterations; i++) {
    results[i] = futures[i].get();
  }
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
  results.clear();
  futures.clear();

  std::cout << "--- test 2: using cyan::dispatch::async_awaitable ---" << std::endl;
  begin = clock::now();
  for (std::size_t i = 0; i < iterations; i++) {
    futures[i] = cyan::dispatch::async_awaitable([i] { return factorial(i); });
  }
  for (std::size_t i = 0; i < iterations; i++) {
    results[i] = futures[i].get();
  }
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
  results.clear();
  futures.clear();

  std::cout << "--- test 3: using cyan::dispatch::async ---" << std::endl;
  begin = clock::now();
  for (std::size_t i = 0; i < iterations; i++) {
    cyan::dispatch::async([i] { return factorial(i); });
  }
  cyan::dispatch::global_concurrent_pool()->stop();
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

  std::cout << "--- test 4: OpenMP execution ---" << std::endl;
  begin = clock::now();
#pragma omp parallel for schedule(dynamic, 1)
  for (std::size_t i = 0; i < iterations; i++) {
    results[i] = factorial(i);
  }
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

  std::cout << "--- test 5: OpenMP with dynamic scheduling ---" << std::endl;
  begin = clock::now();
#pragma omp parallel for schedule(dynamic, 1)
  for (std::size_t i = 0; i < iterations; i++) {
    results[i] = factorial(i);
  }
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

  return 0;
}
