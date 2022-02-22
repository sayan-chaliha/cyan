# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS_INIT "/Wall /WX /std:c++latest")
    set(CMAKE_C_FLAGS_INIT "/Wall /WX")
else()
    set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra -pedantic -Werror -std=c++2b")
    set(CMAKE_C_FLAGS_INIT "-Wall -Wextra -pedantic -Werror")
    add_definitions(-D__CYAN_BUILD_DLL__)
endif()

