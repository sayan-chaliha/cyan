include(CTest)
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
)
if(BUILD_TESTING)
    FetchContent_MakeAvailable(googletest)
endif()

macro(add_test NAME)
    set(MULTI_VALUE_ARGS SOURCES LIBRARIES)
    cmake_parse_arguments(add_test "" "" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    if(BUILD_TESTING)
        add_executable(${NAME} ${add_test_SOURCES})
        set_source_files_properties(${add_test_SOURCES} PROPERTIES COMPILE_FLAGS
            $<$<CXX_COMPILER_ID:AppleClang>:"-Wno-gnu-zero-variadic-macro-arguments">
        )
        target_compile_options(${NAME} PUBLIC
            $<$<CXX_COMPILER_ID:MSVC>:
            >
            $<$<CXX_COMPILER_ID:AppleClang>:
                -fsanitize=address
            >
            $<$<CXX_COMPILER_ID:GNU>:
                #-fsanitize=thread
            >
        )
        target_link_options(${NAME} PUBLIC
            $<$<CXX_COMPILER_ID:MSVC>:
            >
            $<$<CXX_COMPILER_ID:AppleClang>:
                -fsanitize=address
            >
            $<$<CXX_COMPILER_ID:GNU>:
                #-fsanitize=thread
            >
        )
        target_link_libraries(${NAME} ${add_test_LIBRARIES} gmock gtest gtest_main)
        gtest_discover_tests(${NAME})
    endif()
endmacro()
