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
#include <limits>

#include <cyan/dispatch/serial_token.h>

namespace cyan::dispatch {

struct serial_token::serial_token_impl {
  serial_token_impl() : current_sequence{ 0 }, executing_sequence{ 0 } {}

  std::atomic<std::size_t> current_sequence;
  std::atomic<std::size_t> executing_sequence;
};

serial_token::serial_token() : impl_{ std::make_shared<serial_token_impl>() } {
}

bool serial_token::try_acquire(sequence_type seq) {
  return impl_->executing_sequence.load(std::memory_order_acquire) == seq;
}

void serial_token::release() {
  impl_->executing_sequence.fetch_add(1, std::memory_order_release);
}

serial_token::sequence_type serial_token::get_next_sequence() const {
  return impl_->current_sequence.fetch_add(1, std::memory_order_acq_rel);
}

} // cyan::dispatch
