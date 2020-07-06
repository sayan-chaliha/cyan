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
#include <atomic>
#include <exception>

#include <cyan/event.h>
#include <cyan/dispatch/handler_thread.h>


namespace cyan::dispatch {

class default_handler : public handler {
public:
  void on_initialize() override {}
  void on_message(std::any&&) override {}
  void on_finalize() override {}
  void on_error(std::exception&) override {}
};

static default_handler empty_handler;

template<
  template<typename, typename> typename QueueType,
  template<typename> typename Alloc
>
class handler_thread::handler_thread_impl {
public:
  using queue_type = QueueType<std::unique_ptr<detail::message>,
        Alloc<std::unique_ptr<detail::message>>>;

  handler_thread_impl(handler& h) : handler_{ &h },
        queue_{ std::make_unique<queue_type>() },
        stopping_{ false }, running_{ false },
        thread_{ &handler_thread_impl::execute, this } {
  }

  ~handler_thread_impl() {
    stop(true);
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void stop(bool safe) {
    stopping_.store(true, std::memory_order_release);

    if (!is_running()) {
      return;
    } else if (!safe || queue_->empty()) {
      enqueue_unsafe(detail::make_stop_message());
    }
  }

  bool is_stopping() const {
    return stopping_.load(std::memory_order_acquire);
  }

  bool is_running() const {
    return running_.load(std::memory_order_acquire);
  }

  void enqueue(std::unique_ptr<detail::message>&& msg) {
    if (is_stopping()) return;

    queue_->enqueue(std::forward<std::unique_ptr<detail::message>>(msg));

    if (queued_event_) {
      queued_event_->send();
    }
  }

private:
  void enqueue_unsafe(std::unique_ptr<detail::message>&& msg) {
    queue_->enqueue(std::forward<std::unique_ptr<detail::message>>(msg));
    queued_event_->send();
  }

  void execute() {
    if (is_stopping()) return;
    running_.store(true, std::memory_order_release);

    initialize_message_queue();

    try {
      handler_->on_initialize();
    } catch (std::exception& e) {
      handler_->on_error(e);
    }

    // Handle enqueued messages before thread was started
    process_queue();

    cyan::this_thread::get_event_loop()->start();

    try {
      handler_->on_finalize();
    } catch (std::exception& e) {
      handler_->on_error(e);
    }

    running_.store(false, std::memory_order_release);
  }

  void initialize_message_queue() {
    auto loop = cyan::this_thread::get_event_loop();
    queued_event_ = std::make_unique<cyan::event::async>(loop);
    timer_wheel_ = std::make_unique<cyan::event::timer_wheel>(loop);

    queued_event_->start();
    timer_wheel_->start();

    queued_event_->set_callback(&handler_thread_impl::process_queue, this);
  }

  void process_queue() {
    std::unique_ptr<detail::message> msg;
    while (queue_->try_dequeue(msg)) {
      auto timeout = msg->get_timeout();
      if (timeout.count() > 0) {
        timer_wheel_->post([this, msg = std::move(msg)] {
          msg->process(*handler_);
        }, timeout);
      } else {
        if (msg->try_acquire()) {
          msg->process(*handler_);
        } else {
          enqueue(std::move(msg));
        }
      }
    }

    if (is_stopping() && queue_->empty()) {
      cyan::this_thread::get_event_loop()->stop();
    }
  }

  handler* handler_;
  std::unique_ptr<queue_type> queue_;
  std::unique_ptr<cyan::event::async> queued_event_;
  std::unique_ptr<cyan::event::timer_wheel> timer_wheel_;
  std::atomic<bool> stopping_;
  std::atomic<bool> running_;
  std::thread thread_;
};

handler_thread::handler_thread() : impl_{ std::make_unique<default_impl_type>(empty_handler) } {
}

handler_thread::handler_thread(handler& h) : impl_{ std::make_unique<default_impl_type>(h) } {
}

handler_thread::handler_thread(handler_thread&& other) : impl_{ std::move(other.impl_) } {
  other.impl_ = nullptr;
}

handler_thread::~handler_thread() {
}

handler_thread& handler_thread::operator =(handler_thread&& other) {
  impl_ = std::move(other.impl_);
  other.impl_ = nullptr;
  return *this;
}

void handler_thread::stop(bool safe) {
  impl_->stop(safe);
}

bool handler_thread::is_stopping() const {
  return impl_->is_stopping();
}

bool handler_thread::is_running() const {
  return impl_->is_running();
}

void handler_thread::enqueue(std::unique_ptr<detail::message>&& msg) {
  impl_->enqueue(std::forward<std::unique_ptr<detail::message>>(msg));
}

} // cyan::dispatch
