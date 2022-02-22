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
#include <cyan/net/detail/socket_ops.h>
#include <cyan/net/detail/socket_option.h>

namespace cyan::net::detail {

socket_type socket(std::int32_t af, std::int32_t type, std::int32_t protocol, std::error_code& ec) noexcept {
  clear_last_error();

  socket_type sock = error_wrapper(::socket(af, type, protocol), ec);
  if (sock == invalid_socket) return sock;

#ifdef CYAN_OS_DEF_SO_NOSIGPIPE
  boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_NOSIGPIPE)> nosigpipe{ true };
  if (set_socket_option(sock, nosigpipe, ec) == -1) {
    ::close(sock);
    return invalid_socket;
  }
#endif // CYAN_OS_DEF_SO_NOSIGPIPE

  if (type == CYAN_OS_DEF(SOCK_STREAM)) {
    boolean_socket_option<CYAN_OS_DEF(IPPROTO_TCP), CYAN_OS_DEF(TCP_NODELAY)> nodelay{ true };
    if (set_socket_option(sock, nodelay, ec) == -1) {
      ::close(sock);
      return invalid_socket;
    }
  }

  if (socket_set_non_blocking(sock, true, ec) == -1) {
    ::close(sock);
    return invalid_socket;
  }

  ec = std::error_code{};

  return sock;
}

socket_type socket_dup(socket_type sock, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return sock;
  }

  clear_last_error();
#ifdef WIN32
  return error_wrapper(::_dup(sock), ec);
#else
  return error_wrapper(::dup(sock), ec);
#endif // WIN32
}

std::int32_t socket_set_non_blocking(socket_type sock, bool onoff, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return sock;
  }

  clear_last_error();
#ifdef WIN32
  unsigned long mode = onoff ? 1 : 0;
  return error_wrapper(::ioctlsocket(sock, FIONBIO, &mode), ec);
#else
  std::int32_t flags = error_wrapper(::fcntl(sock, F_GETFL, 0), ec);
  if (flags == -1) return flags;
  flags = onoff ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
  return error_wrapper(::fcntl(sock, F_SETFL, flags), ec);
#endif // WIN32
}

std::int32_t socket_get_non_blocking(socket_type sock, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return sock;
  }

  clear_last_error();
#ifdef WIN32
  unsigned long mode = onoff ? 0 : 1;
  return error_wrapper(::ioctlsocket(sock, FIONBIO, &mode), ec);
#else
  std::int32_t flags = error_wrapper(::fcntl(sock, F_GETFL, 0), ec);
  if (flags == -1) return flags;
  return flags & O_NONBLOCK;
#endif // WIN32
}


std::int32_t shutdown(socket_type sock, std::int32_t how, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::shutdown(sock, how), ec);
}

std::int32_t bind(socket_type sock, sockaddr_type const* addr, std::uint32_t len, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::bind(sock, addr, len), ec);
}

std::int32_t listen(socket_type sock, std::int32_t backlog, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::listen(sock, backlog), ec);
}

std::int32_t connect(socket_type sock, sockaddr_type const* addr, std::uint32_t len, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::connect(sock, addr, len), ec);
}

socket_type accept(socket_type sock, sockaddr_type* addr, std::uint32_t* len, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  socket_type new_sock = error_wrapper(::accept(sock, addr, len), ec);
  if (new_sock == invalid_socket) return new_sock;
 
#ifdef CYAN_OS_DEF_SO_NOSIGPIPE
  boolean_socket_option<CYAN_OS_DEF(SOL_SOCKET), CYAN_OS_DEF(SO_NOSIGPIPE)> nosigpipe{ true };
  if (set_socket_option(new_sock, nosigpipe, ec) == -1) {
    ::close(new_sock);
    return invalid_socket;
  }
#endif // CYAN_OS_DEF_SO_NOSIGPIPE

  boolean_socket_option<CYAN_OS_DEF(IPPROTO_TCP), CYAN_OS_DEF(TCP_NODELAY)> nodelay{ true };
  if (set_socket_option(new_sock, nodelay, ec) == -1) {
    ::close(new_sock);
    return invalid_socket;
  }

  if (socket_set_non_blocking(new_sock, true, ec) == -1) {
    ::close(new_sock);
    return invalid_socket;
  }

  ec = std::error_code{};

  return new_sock;
}

std::int32_t close(socket_type sock, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::close(sock), ec);
}

std::int64_t receive(socket_type sock, void* buffer, std::size_t len, std::int32_t flags, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::recv(sock, buffer, len, flags), ec);
}

std::int64_t send(socket_type sock, void const* buffer, std::size_t len, std::int32_t flags, std::error_code& ec) noexcept {
  if (sock == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return -1;
  }

  clear_last_error();
  return error_wrapper(::send(sock, buffer, len, flags), ec);
}


} // cyan::net::detail
