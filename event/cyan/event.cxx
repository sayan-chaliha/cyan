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
#include <mutex>
#include <thread>
#include <memory>

#include <cyan/event.h>
#include <cyan/event/basic_loop.txx>
#include <cyan/event/basic_async.txx>
#include <cyan/event/basic_io.txx>

namespace {

static std::thread::id main_thread_id = std::this_thread::get_id();

}

namespace cyan::event {
inline namespace v1 {

std::shared_ptr<event::loop> get_main_loop() {
  static std::shared_ptr<event::loop> loop;
  static std::once_flag once_flag;

  std::call_once(once_flag, [&] {
    loop = std::make_shared<event::loop>();
  });

  return loop;
}

template class basic_loop<default_backend_traits>;
template class basic_async<default_backend_traits>;
template class basic_idle<default_backend_traits>;
template class basic_timer<default_backend_traits>;
template class basic_timer_wheel<default_backend_traits>;
template class basic_io<default_backend_traits>;

} // v1
} // cyan::event

namespace cyan::this_thread {
inline namespace v1 {

std::shared_ptr<event::loop> get_event_loop() {
  if (std::this_thread::get_id() == main_thread_id) {
    return cyan::event::get_main_loop();
  }

  static thread_local std::shared_ptr<event::loop> loop;
  static thread_local std::once_flag once_flag;

  std::call_once(once_flag, [&] {
    loop = std::make_shared<event::loop>();
  });

  return loop;
}

} // v1
} // cyan::this_thread
