include(Macros)

set(LIB_NAME ${PROJECT_NAME}_net)

set(HEADERS
    cyan/net/detail/platform.h
    cyan/net/detail/socket_option.h
    cyan/net/detail/socket_ops.h
    cyan/net/detail/address.h
    cyan/net/ip/detail/endpoint.h
    cyan/net/ip/address_v4.h
    cyan/net/ip/address.h
    cyan/net/ip/basic_endpoint.h
    cyan/net/ip/tcp.h
)
set(SOURCES
    ${HEADERS}
    cyan/net/detail/socket_ops.cxx
    cyan/net/detail/address.cxx
    cyan/net/ip/detail/endpoint.cxx
    cyan/net/ip/address_v4.cxx
    cyan/net/ip/address.cxx
)

add_library(${LIB_NAME} STATIC ${SOURCES})

target_link_libraries(${LIB_NAME} PUBLIC
    ${PROJECT_NAME}_event)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR})

install_headers("${HEADERS}")
install(TARGETS ${LIB_NAME})

