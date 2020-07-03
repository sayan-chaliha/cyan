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
#include <iostream>
#include <sstream>

#include <cyan/dispatch.h>
using namespace std::chrono_literals;
using namespace std::string_literals;

// This active object pattern example uses its own
// thread to execute functions, and process messages.
class active_object {
public:
  active_object() {
  }

  ~active_object() {
  }

  void process_on_background_thread() {
    std::stringstream sstream;
    sstream << "hello from " << std::this_thread::get_id();
    cyan::dispatch::async(serial_token_, [str = sstream.str()] {
      std::cout << std::this_thread::get_id() << ": " << str << std::endl;
    });
  }

  void execute_on_background_thread() {
    cyan::dispatch::async(serial_token_, [] {
      std::cout << "hello world!" << std::endl;
    });
  }

  void error_on_background_thread() {
    // Errors on the background thread are suppressed
    cyan::dispatch::async(serial_token_, [] {
      throw std::runtime_error("something went wrong");
    });
  }

  std::future<int> wait_for_result() {
    return cyan::dispatch::async_awaitable(serial_token_, [] {
      std::cout << "wait for result" << std::endl;
      return 10;
    });
  }

private:
  cyan::dispatch::serial_token serial_token_;
};

int main() {
  active_object obj;
  // Send a message
  obj.process_on_background_thread();

  // Wait for result
  auto f = obj.wait_for_result();

  // Execute a function on the background thread
  obj.execute_on_background_thread();

  // Regardless, everything is executed serially
  // on the handler thread
  std::cout << f.get() << std::endl;

  // Error handling
  obj.error_on_background_thread();

  std::this_thread::sleep_for(2s);
  return 0;
}
