include(Macros)

find_package(Protobuf REQUIRED)

set(LIB_NAME ${PROJECT_NAME}_messages)

file(GLOB_RECURSE IDL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/idl/*)

protobuf_generate_cpp(SOURCES HEADERS DESCRIPTORS DESCS ${IDL_FILES})
add_library(${LIB_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(${LIB_NAME} PUBLIC
    $<$<CXX_COMPILER_ID:AppleClang>:
        -Wno-unused-parameter
        -Wno-zero-length-array
    >)
target_link_libraries(${LIB_NAME} ${Protobuf_LIBRARIES})
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME}/messages)
