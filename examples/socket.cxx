#include <cyan/net/ip/tcp.h>
#include <cyan/net/buffers.h>
#include <cyan/event.h>
#include <iostream>
#include <vector>

int main() {
  auto address = cyan::net::ip::make_address("127.0.0.1");
  cyan::net::ip::tcp::endpoint endpoint{ address, 6666 };
  cyan::net::ip::tcp::acceptor sock{ cyan::this_thread::get_event_loop(), endpoint };

  std::thread thread1{[handle = sock.native_handle()] {
    try {
      cyan::net::ip::tcp::acceptor sock{ cyan::this_thread::get_event_loop(), handle };
      sock.start();

      sock.set_connection_callback([] (std::unique_ptr<cyan::net::ip::tcp::socket>&& sock) {
        std::cout << std::this_thread::get_id() << ": " << sock->remote_endpoint() << std::endl;
        sock->send(std::string("hello world\n"));
        std::vector<char> buf;
        buf.reserve(256); 
        try {
          sock->receive(cyan::net::mutable_buffer(buf));
        } catch (std::system_error& error) {
          std::cout << error.code() << ": " << error.what() << std::endl;
        }
        sock->shutdown(cyan::net::ip::tcp::socket::shutdown_type::both);
      });

      cyan::this_thread::get_event_loop()->start();
    } catch (std::system_error ec) {
      std::cout << ec.code() << std::endl;
    }
  }};

  thread1.join();
  return 0;
}
