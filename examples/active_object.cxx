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
class active_object : public cyan::dispatch::handler {
public:
  active_object() : thread_{ *this } {
  }

  ~active_object() {
    thread_.stop();
  }

  void process_on_background_thread() {
    std::stringstream sstream;
    sstream << "hello from " << std::this_thread::get_id();
    thread_.send(sstream.str());
  }

  void execute_on_background_thread() {
    thread_.post([] {
      std::cout << "hello world!" << std::endl;
    });
  }

  void error_on_background_thread() {
    thread_.post([] {
      throw std::runtime_error("something went wrong");
    });
  }

  std::future<void> wait_for_result() {
    return thread_.post_awaitable([] {
      std::cout << "wait for result" << std::endl;
    });
  }

protected:
  void on_initialize() override {
    std::cout << std::this_thread::get_id() << ": initialized" << std::endl;
  }

  void on_message(std::any&& message) override {
    std::cout << std::this_thread::get_id() << ": received message: "
          << std::any_cast<std::string>(message) << std::endl;
  }

  void on_finalize() override {
    std::cout << std::this_thread::get_id() << ": finalized" << std::endl;
  }

  void on_error(std::exception& e) noexcept override {
    std::cout << std::this_thread::get_id() << ": error: " << e.what() << std::endl;
  }

private:
  cyan::dispatch::handler_thread thread_;
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
  f.get();

  // Error handling
  obj.error_on_background_thread();

  std::this_thread::sleep_for(2s);
  return 0;
}
