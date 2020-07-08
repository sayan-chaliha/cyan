#include <cyan/net/ip/tcp.h>
#include <iostream>

int main() {
  auto address = cyan::net::ip::make_address("127.0.0.1");
  cyan::net::ip::tcp::endpoint_type endpoint{ address, 6666 };
  try {
    cyan::net::ip::tcp::socket_type sock{ endpoint };
    std::cout << sock.native_handle() << std::endl;
  } catch (std::system_error ec) {
    std::cout << ec.code() << std::endl;
  }
  return 0;
}
