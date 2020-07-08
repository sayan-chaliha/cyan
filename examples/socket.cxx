#include <cyan/net/ip/tcp.h>
#include <iostream>

int main() {
  auto address = cyan::net::ip::make_address("127.0.0.1");
  cyan::net::ip::tcp::endpoint endpoint{ address, 5050 };
  std::cout << endpoint << std::endl;
  return 0;
}
