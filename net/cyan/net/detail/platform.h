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
#define CYAN_OS_DEF_MSG_PEEK MSG_PEEK
#define CYAN_OS_DEF_MSG_OOB MSG_OOB
#define CYAN_OS_DEF_MSG_EOR MSG_EOR
#define CYAN_OS_DEF_MSG_DONTROUTE MSG_DONTROUTE
#define CYAN_OS_DEF_INADDR_ANY INADDR_ANY
#define CYAN_OS_DEF_SHUT_RD SHUT_RD
#define CYAN_OS_DEF_SHUT_WR SHUT_WR
#define CYAN_OS_DEF_SHUT_RDWR SHUT_RDWR

#define CYAN_OS_DEF_SOMAXCONN SOMAXCONN
#define CYAN_OS_DEF_SOL_SOCKET SOL_SOCKET
#define CYAN_OS_DEF_SO_DEBUG SO_DEBUG // boolean
#define CYAN_OS_DEF_SO_REUSEADDR SO_REUSEADDR // boolean
#define CYAN_OS_DEF_SO_REUSEPORT SO_REUSEPORT // boolean
#define CYAN_OS_DEF_SO_KEEPALIVE SO_KEEPALIVE // boolean
#define CYAN_OS_DEF_SO_DONTROUTE SO_DONTROUTE // boolean
#define CYAN_OS_DEF_SO_LINGER SO_LINGER // linger
#define CYAN_OS_DEF_SO_BROADCAST SO_BROADCAST // boolean
#define CYAN_OS_DEF_SO_OOBINLINE SO_OOBINLINE // boolean
#define CYAN_OS_DEF_SO_SNDBUF SO_SNDBUF // integer
#define CYAN_OS_DEF_SO_RCVBUF SO_RCVBUF // integer
#define CYAN_OS_DEF_SO_SNDLOWAT SO_SNDLOWAT // integer
#define CYAN_OS_DEF_SO_RCVLOWAT SO_RCVLOWAT // integer
#define CYAN_OS_DEF_SO_SNDTIMEO SO_SNDTIMEO // time
#define CYAN_OS_DEF_SO_RCVTIMEO SO_RCVTIMEO // time
#define CYAN_OS_DEF_SO_TYPE SO_TYPE // integer
#define CYAN_OS_DEF_SO_ERROR SO_ERROR // integer
#ifdef SO_NOSIGPIPE
# define CYAN_OS_DEF_SO_NOSIGPIPE SO_NOSIGPIPE // boolean
#endif // SO_NOSIGPIPE
#define CYAN_OS_DEF_SO_NREAD SO_NREAD // integer
#define CYAN_OS_DEF_SO_NWRITE SO_NWRITE // integer
#define CYAN_OS_DEF_SO_LINGER_SEC SO_LINGER_SEC

#define CYAN_OS_DEF_TCP_NODELAY TCP_NODELAY // boolean
#define CYAN_OS_DEF_TCP_MAXSEG TCP_MAXSEG
#define CYAN_OS_DEF_TCP_NOOPT TCP_NOOPT
#define CYAN_OS_DEF_TCP_NOPUSH TCP_NOPUSH
#define CYAN_OS_DEF_TCP_KEEPALIVE TCP_KEEPALIVE
#define CYAN_OS_DEF_TCP_CONNECTIONTIMEOUT TCP_CONNECTIONTIMEOUT
#define CYAN_OS_DEF_TCP_KEEPINTVL TCP_KEEPINTVL
#define CYAN_OS_DEF_TCP_KEEPCNT TCP_KEEPCNT

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
