include(Macros)
include(GoogleTest)

set(LIB_NAME ${PROJECT_NAME}_dispatch)

set(HEADERS
    cyan/dispatch.h
    cyan/dispatch/handler_thread.h
    cyan/dispatch/handler.h
    cyan/dispatch/message.h
    cyan/dispatch/thread_pool.h
    cyan/dispatch/async.h
    cyan/dispatch/serial_token.h
)
set(SOURCES
    ${HEADERS}
    cyan/dispatch/handler_thread.cxx
    cyan/dispatch/message.cxx
    cyan/dispatch/thread_pool.cxx
    cyan/dispatch/serial_token.cxx
)
set(SOURCES_TEST
    test/async_tests.cxx
)

add_library(${LIB_NAME} STATIC ${SOURCES})

target_link_libraries(${LIB_NAME} PUBLIC
    ${PROJECT_NAME}_common
    ${PROJECT_NAME}_lockfree
    ${PROJECT_NAME}_event
)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR})

install_headers("${HEADERS}")
install(TARGETS ${LIB_NAME})

add_test(${LIB_NAME}_test SOURCES ${SOURCES_TEST} LIBRARIES ${LIB_NAME})
