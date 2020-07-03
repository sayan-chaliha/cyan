# Platform checks
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckStructHasMember)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckCXXSourceCompiles)

# Setting HAVE_STRUCT_<name>
FUNCTION(check_struct_size type defbase)
  check_type_size("struct ${type}" SIZEOF_${defbase})
  IF(SIZEOF_${defbase})
    SET(HAVE_STRUCT_${defbase} 1 PARENT_SCOPE)
  ENDIF()
ENDFUNCTION()

# Check includes
macro(cyan_check_include_files file variable)
  check_include_files("${file}" ${variable})
  if(${variable})
    list(APPEND CMAKE_EXTRA_INCLUDE_FILES "${file}")
  endif()
endmacro()

# Include files
cyan_check_include_files(dlfcn.h HAVE_DLFCN_H)
cyan_check_include_files(inttypes.h HAVE_INTTYPES_H)
cyan_check_include_files(memory.h HAVE_MEMORY_H)
cyan_check_include_files(poll.h HAVE_POLL_H)
cyan_check_include_files(port.h HAVE_PORT_H)
cyan_check_include_files(stdint.h HAVE_STDINT_H)
cyan_check_include_files(stdlib.h HAVE_STDLIB_H)
cyan_check_include_files(strings.h HAVE_STRINGS_H)
cyan_check_include_files(string.h HAVE_STRING_H)
cyan_check_include_files("sys/epoll.h" HAVE_SYS_EPOLL_H)
cyan_check_include_files("sys/eventfd.h" HAVE_SYS_EVENTFD_H)
cyan_check_include_files("sys/event.h" HAVE_SYS_EVENT_H)
cyan_check_include_files("sys/inotify.h" HAVE_SYS_INOTIFY_H)
cyan_check_include_files("sys/select.h" HAVE_SYS_SELECT_H)
cyan_check_include_files("sys/signalfd.h" HAVE_SYS_SIGNALFD_H)
cyan_check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
cyan_check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
cyan_check_include_files(unistd.h HAVE_UNISTD_H)
cyan_check_include_files("netinet/in.h" HAVE_NETINET_IN_H)
cyan_check_include_files("netinet/in6.h" HAVE_NETINET_IN6_H)
cyan_check_include_files(netdb.h HAVE_NETDB_H)
cyan_check_include_files("sys/socket.h" HAVE_SYS_SOCKET_H)
cyan_check_include_files("winsock2.h;windows.h" HAVE_WINSOCK2_H)
cyan_check_include_files("winsock2.h;ws2tcpip.h;windows.h" HAVE_WS2TCPIP_H)
cyan_check_include_files("winsock.h;windows.h" HAVE_WINSOCK_H)
cyan_check_include_files(windows.h HAVE_WINDOWS_H)

check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
check_function_exists(epoll_ctl HAVE_EPOLL_CTL)
check_function_exists(eventfd HAVE_EVENTFD)
check_function_exists(floor HAVE_FLOOR)
check_function_exists(inotify_init HAVE_INOTIFY_INIT)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(poll HAVE_POLL)
check_function_exists(port_create HAVE_PORT_CREATE)
check_function_exists(select HAVE_SELECT)
check_function_exists(signalfd HAVE_SIGNALFD)

# Libraries
find_library(HAVE_LIBRT rt)
find_library(HAVE_LIBC c)

# structs
check_struct_size("sockaddr" SOCKADDR)
check_struct_size("sockaddr_storage" SOCKADDR_STORAGE)
check_struct_size("sockaddr_in" SOCKADDR_IN)
check_struct_size("in_addr" IN_ADDR)
check_struct_size("sockaddr_in6" SOCKADDR_IN6)
check_struct_size("in6_addr" IN6_ADDR)

# Symbols
check_cxx_source_compiles(
    "
    #include <new>

    int main() {
      return std::hardware_destructive_interference_size;
    }
    "
    HAVE_HARDWARE_DESTRUCTIVE_INTERFERENCE_SIZE
)
check_cxx_source_compiles(
    "
    void func() __attribute__((visibility(\"hidden\")));
    int main() { return 0; }
    "
    HAVE___ATTRIBUTE__
)
check_cxx_source_compiles(
    "
    void func() __declspec(export);
    int main() { return 0; }
    "
    HAVE___DECLPSEC
)

# Tweaks
set(HAVE_CLOCK_SYSCALL ${HAVE_CLOCK_GETTIME})

