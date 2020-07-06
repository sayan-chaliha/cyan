#include <boost/asio.hpp>
#include <cyan/dispatch/handler_thread.h>

#include <thread>
#include <iostream>

// Factorial function
std::atomic_int call_count = 0;

std::size_t factorial(std::size_t i) {
  call_count++;
  i = std::max(1ul, i);

  std::size_t r = i;
  while (i > 2) {
    r *= --i;
  }

  return r;
}

int main() {
  using clock = std::chrono::high_resolution_clock;
  clock::time_point begin, end;
  constexpr int iters = 30'000'000;
  std::shared_ptr<cyan::dispatch::handler_thread> handler_thread = std::make_shared<cyan::dispatch::handler_thread>();
  boost::asio::io_context io_context;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard
      = boost::asio::make_work_guard(io_context);

  std::thread thread{ [&io_context] { io_context.run(); }};

  std::cout << "---- test 1 using handler_thread ----" << std::endl;
  begin = clock::now();
  for (auto i = 0; i < iters; i++) {
    handler_thread->post([i] { factorial(i); });
  }
  handler_thread.reset();
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
  std::cout << "Call count: " << call_count << std::endl;
  call_count = 0;

  std::cout << "---- test 2 using boost::asio ----" << std::endl;
  begin = clock::now();
  for (auto i = 0; i < iters; i++) {
    boost::asio::post(io_context, [i] { factorial(i); });
  }
  work_guard.reset();
  thread.join();
  end = clock::now();
  std::cout << "Execution time: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
  std::cout << "Call count: " << call_count << std::endl;

  return 0;
}
