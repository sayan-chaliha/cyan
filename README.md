# cyan
#### A lock-free, wait-free, event-driven, cross-platform, thread pool-backed asynchronous execution library in C++.
Actually, __cyan__ is a collection of smaller libraries that you can use separately as well.
- ```common``` contains generic utilities. 
- ```event``` contains C++11 abstractions over event libraries (currently only specialized for [libev](http://software.schmorp.de/), but hopefully easy enough to extend to [libevent](https://libevent.org)).
- ```lockfree``` contains my experiment with [Michael and Scott's Queue](http://web.cecs.pdx.edu/~walpole/class/cs510/papers/08.pdf), and an implementation of a lock-free ```stack``` based on Anthony Williams's book, [Concurrency in C++](https://g.co/kgs/QHfKCn).
- ```dispatch``` combines ```event``` and ```lockfree``` with threads and implements asynchronous dispatch.

Back in 2014, I worked for a telecom company where I had proposed an application architecture based solely on threads running event loops and responding to events, whether they be network events or messages from other threads. It was implemented using fixed-size SPSC lock-free queues. That architecture made it to production and handled over 60K TPS HTTP transactions on a single ATCA chassis blade.

Library components required to build such applications in C++ are few, so I hacked this project together in my free time.

API documentation is shoddy, or absent at best (I'm still working on that), but hopefully the examples will help.
## Benchmark
Here are the results of running the ```factorial_benchmark``` found in the ```examples``` on my Mac Book Air.
> % ./examples/factorial_benchmark  
> --- test 1: using std::async ---  
> Execution time: 15736ms  
> --- test 2: using cyan::dispatch::async_awaitable ---  
> Execution time: 12352ms  
> --- test 3: using cyan::dispatch::async ---  
> Execution time: 160ms  
> --- test 4: OpenMP execution ---  
> Execution time: 1743ms  
> --- test 5: OpenMP with dynamic scheduling ---  
> Execution time: 1806ms  
> 
> % sysctl -n machdep.cpu.brand_string  
> Intel(R) Core(TM) i7-4650U CPU @ 1.70GHz  
> 
> % sysctl -n machdep.cpu.core_count  
> 2
>
> % clang -v  
> Apple clang version 11.0.3 (clang-1103.0.32.62)  
> Target: x86_64-apple-darwin19.5.0  
> Thread model: posix  
> InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin  
## Building
```
% mkdir -p build
% cmake ..
% make
```
Although the libraries should compile on other platforms, they may require tweaks to ```CMakeLists.txt``` because I have not tested on Linux and Windows yet.
## Examples
Full examples are available in the source tree.
### Dispatch
##### Execute on arbitrary background thread
```c++
cyan::dispatch::async([] {
  // your code here
});
```
##### Execute tasks serially on an arbitrary background thread
```c++
cyan::dispatch::serial_token token;

cyan::dispatch::async(token, [] {
  // First task
});

cyan::dispatch::async(token, [] {
  // Task executed only after first task finishes
});
```
##### Futures
```c++
auto future = cyan::dispatch::async_awaitable([] {
  // perform task
  return result;
});
auto result = future.get();
```
##### Handler threads
```c++
class active_object : public cyan::dispatch::handler {
public:
  active_object() : thread_{ *this } {
  }

  ~active_object() {
    thread_.stop();
  }

  void process_on_background_thread() {
  	// ...
    thread_.send(message);
  }

  void execute_on_background_thread() {
    thread_.post([] {
   	  // ...
    });
  }

  std::future<void> wait_for_result() {
    return thread_.post_awaitable([] {
      // ...
    });
  }

protected:
  void on_initialize() override {
  	// Background thread has started
  }

  void on_message(std::any&& message) override {
  	// Callback on `handler_thread::send() or handler_thread::send_delayed()`
  }

  void on_finalize() override {
  	// Handler thread is exiting
  }

  void on_error(std::exception& e) noexcept override {
  	// Exception on background thread
  }

private:
  cyan::dispatch::handler_thread thread_;
};
```
### Event
##### Timer
```c++
cyan::event::timer timer{ cyan::this_thread::get_event_loop() };
timer.set_timeout(500ms);
timer.set_callback([] {
  // 500ms expired
});
timer.start();

// do some work
timer.reset(); // reset timer on activity
  
cyan::this_thread::get_event_loop()->start();
```
##### Timer wheel -- when you have more timers than you can count
```c++
cyan::event::timer_wheel timer_wheel{ cyan::this_thread::get_event_loop() };
timer_wheel.start();

auto id = timer_wheel.post([] { /** ... */ }, 500ms);
// ...
timer_wheel.cancel(id); // cancels if not already fired
```
##### Async event -- wake up the event loop (even from another thread)
```c++
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
  
cyan::event::get_main_loop()->start();
```
##### Idle event -- execute when event loop is idle
```c++
cyan::event::idle idle{ cyan::event::get_main_loop() };
idle.set_callback([&idle] {
  std::cout << "i'm idle" << std::endl;

  // Don't want it firing continually.
  idle.stop();
});
idle.start();

// Starts and blocks on the event loop.
cyan::event::get_main_loop()->start();
```
##### Signal event -- signal handlers as a callback
```c++
cyan::event::signal signal{ cyan::this_thread::get_event_loop() };
signal.set_number(SIGINT);
signal.set_callback([] {
  std::cout << "signal SIGINT received. Shutting down loop." << std::endl;

  // Every thread gets its own event loop. This
  // call returns the main event loop itself.
  cyan::this_thread::get_event_loop()->stop();
});
signal.start();

cyan::event::get_main_loop()->start();
```
### ToDos
- [ ] Unit tests
- [ ] Documentation

