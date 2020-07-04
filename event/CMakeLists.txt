include(Macros)
include(GoogleTest)

set(LIB_NAME ${PROJECT_NAME}_event)

set(HEADERS
    cyan/event.h
    cyan/event/basic_loop.h
    cyan/event/basic_async.h
    cyan/event/basic_timer.h
    cyan/event/basic_signal.h
    cyan/event/basic_idle.h
    cyan/event/backend.h
    cyan/event/backend_libev.h
    cyan/external/ev/ev.h
)
set(SOURCES
    ${HEADERS}
    cyan/event.cxx
    cyan/event/backend_libev.cxx
)
set(SOURCES_TEST
    test/event_tests.cxx
)

add_library(${LIB_NAME} STATIC ${SOURCES})
target_link_libraries(${LIB_NAME} PUBLIC ${PROJECT_NAME}_common)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR})

add_test(${LIB_NAME}_test SOURCES ${SOURCES_TEST} LIBRARIES ${LIB_NAME})

install_headers(${HEADERS})
install(TARGETS ${LIB_NAME})
