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

#include <system_error>
#include <functional>

#include <cyan/config.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif // HAVE_SYS_TYPES_H

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif // HAVE_NETINET_IN_H

#ifdef HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif // HAVE_NETINET_IN6_H

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif // HAVE_NETDB_H

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif // HAVE_SYS_SOCKET_H

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif // HAVE_WINDOWS_H

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif // HAVE_WINSOCK2_H

#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif // HAVE_WS2TCPIP_H

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif // HAVE_WINSOCK_H

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif // HAVE_FCNTL_H

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif // HAVE_ARPA_INET_H

#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif // HAVE_NETINET_TCP_H

#ifdef HAVE_NETINET_TCP_VAR_H
#include <netinet/tcp_var.h>
#endif // HAVE_NETINET_TCP_VAR_H

#define CYAN_OS_DEF(c) CYAN_OS_DEF_##c
#define CYAN_OS_DEF_AF_INET AF_INET
#define CYAN_OS_DEF_AF_INET6 AF_INET6
#define CYAN_OS_DEF_IPPROTO_TCP IPPROTO_TCP
#define CYAN_OS_DEF_SOCK_STREAM SOCK_STREAM
#define CYAN_OS_DEF_SOMAXCONN SOMAXCONN

namespace cyan::net::detail {

using socket_type = decltype(std::function<decltype(::socket)>{})::result_type;
constexpr socket_type invalid_socket = -1;

inline void clear_last_error() {
#if defined(WIN32) || defined(__CYGWIN__)
  WSASetLastError(0);
#else
  errno = 0;
#endif
}

template<typename ReturnType>
ReturnType error_wrapper(ReturnType val, std::error_code& err) {
#if defined(WIN32) || defined(__CYGWIN__)
  err = std::error_code{ WSAGetLastError(), std::system_category() };
#else
  err = std::error_code{ errno, std::system_category() };
#endif
  return val;
}

}
