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
#include <thread>
#include <iostream>

#include <cyan/event.h>
using namespace std::chrono_literals;

int main(int, char**) {
  // Async events are a mechanism of waking up the loop
  // (from other threads if necessary.
  cyan::event::async async{ cyan::event::get_main_loop() };
  async.set_callback([] {
    std::cout << "i'm awake, i'm awake!!" << std::endl;
  });
  async.start();

  std::thread thread{ [&] {
    std::this_thread::sleep_for(1s);
    async.send();
  }};
  thread.detach();

  // Timers fire at regular intervals until stopped.
  // Guarantees that the timer will fire on or after
  // the timeout (not before). Use ``timer.reset()``
  // to restart the timer.
  cyan::event::timer timer{ cyan::event::get_main_loop() };
  timer.set_timeout(500ms);
  timer.set_callback([] {
    static int ticks = 0;
    std::cout << "ticks = " << ticks++ << std::endl;
  });
  timer.start();

  // Signal handlers can catch signals sent to the process.
  // The same signal cannot be handled by multiple threads
  // simultaneously.
  cyan::event::signal signal{ cyan::this_thread::get_event_loop() };
  signal.set_number(SIGINT);
  signal.set_callback([] () {
    std::cout << "signal SIGINT received. Shutting down loop." << std::endl;

    // Every thread gets its own event loop. This
    // call returns the main event loop itself.
    cyan::this_thread::get_event_loop()->stop();
  });
  signal.start();

  // Idle events run when an iteration of a loop does no useful
  // work. Chances are, if handled incorrectly, idle events will
  // fire continually.
  cyan::event::idle idle{ cyan::event::get_main_loop() };
  idle.set_callback([&idle] {
    std::cout << "i'm idle" << std::endl;

    // Don't want it firing continually.
    idle.stop();
  });
  idle.start();

  // Starts and blocks on the event loop.
  cyan::event::get_main_loop()->start();

  return 0;
}
