include(Macros)

set(LIB_NAME ${PROJECT_NAME}_common)

set(HEADERS
    cyan/cyan.h
    cyan/utility.h
    cyan/noncopyable.h
)
set(SOURCES
    ${HEADERS}
    cyan/cyan.cxx
)

add_library(${LIB_NAME} SHARED ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR})

install_headers("${HEADERS}")
install(TARGETS ${LIB_NAME})

