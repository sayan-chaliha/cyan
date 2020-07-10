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

#include <cyan/event/backend_libev.h>
#include <cyan/event/basic_loop.h>
#include <cyan/event/basic_async.h>
#include <cyan/event/basic_timer.h>
#include <cyan/event/basic_timer_wheel.h>
#include <cyan/event/basic_signal.h>
#include <cyan/event/basic_idle.h>
#include <cyan/event/basic_io.h>

namespace cyan::event {
inline namespace v1 {

using default_backend_traits = backend_traits<backend::libev>;

using loop = basic_loop<default_backend_traits>;
using async = basic_async<default_backend_traits>;
using timer = basic_timer<default_backend_traits>;
using timer_wheel = basic_timer_wheel<default_backend_traits>;
using signal = basic_signal<default_backend_traits>;
using idle = basic_idle<default_backend_traits>;
using io = basic_io<default_backend_traits>;

std::shared_ptr<loop> get_main_loop();

} // v1
} // cyan::event

namespace cyan::this_thread {
inline namespace v1 {

std::shared_ptr<event::loop> get_event_loop();

}
}
