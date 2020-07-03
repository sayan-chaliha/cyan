include(Macros)
include(GoogleTest)

set(LIB_NAME ${PROJECT_NAME}_lockfree)

set(HEADERS
    cyan/lockfree/queue.h
    cyan/lockfree/stack.h
    cyan/lockfree/tagged_ptr.h
    cyan/lockfree/freelist.h
)

add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(${LIB_NAME} INTERFACE ${PROJECT_NAME}_common)

install_headers("${HEADERS}")

### TEST ###
#set(SOURCES_TEST
#)
#
#add_executable(${LIB_NAME}_test ${SOURCES_TEST})
#target_link_libraries(${LIB_NAME}_test gtest)
#gtest_discover_tests(${LIB_NAME}_test)
