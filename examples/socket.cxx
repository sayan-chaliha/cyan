#include <cyan/net/ip/tcp.h>
#include <cyan/net/buffers.h>
#include <cyan/event.h>
#include <iostream>
#include <vector>

int func(int val) {
  return val;
}

int main() {
  auto address = cyan::net::ip::make_address("127.0.0.1");
  cyan::net::ip::tcp::endpoint endpoint{ address, 6666 };
  cyan::net::ip::tcp::acceptor sock{ cyan::this_thread::get_event_loop(), endpoint };

  std::thread thread1{[handle = sock.native_handle()] {
    try {
      cyan::net::ip::tcp::acceptor sock{ cyan::this_thread::get_event_loop(), handle };
      std::vector<std::shared_ptr<cyan::net::ip::tcp::socket>> connected_sockets;

      sock.start();

      sock.set_connection_callback([&connected_sockets] (std::unique_ptr<cyan::net::ip::tcp::socket> sock) {
        std::cout << std::this_thread::get_id() << ": " << sock->remote_endpoint() << std::endl;
        sock->send(std::string("> Hello, socket connection. How are you?\n"));

        sock->set_readable_callback([] (cyan::net::ip::tcp::socket& sock) {
          std::string data{ "\0", 25 };
          try {
            auto read = sock.receive(cyan::net::mutable_buffer(data));
            std::cout << "Read " << read << " bytes." << std::endl;
            std::cout << data << std::endl;
          } catch (std::system_error& error) {
            std::cout << error.code() << ": " << error.what() << std::endl;
          }
          sock.shutdown(cyan::net::ip::tcp::socket::shutdown_type::both);
          sock.stop();
        });

        sock->start();
        connected_sockets.push_back(std::move(sock));
      });

      cyan::this_thread::get_event_loop()->start();
    } catch (std::system_error ec) {
      std::cout << ec.code() << std::endl;
    }
  }};

  thread1.join();
  return 0;
}
