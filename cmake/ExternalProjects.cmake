include(ExternalProject)

set_directory_properties(PROPERTIES
    EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
)

if(BUILD_TESTING)
    ExternalProject_Add(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
        GIT_PROGRESS true
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libgtest.a <INSTALL_DIR>/lib/libgmock.a
    )
    ExternalProject_Get_Property(googletest INSTALL_DIR)
    ExternalProject_Get_Property(googletest SOURCE_DIR)
    if(WIN32)
      set(GTEST_STATIC_LIB ${INSTALL_DIR}/lib/gtest.lib)
      set(GMOC_STATIC_LIB ${INSTALL_DIR}/lib/gmock.lib)
    else()
      set(GTEST_STATIC_LIB ${INSTALL_DIR}/lib/libgtest.a)
      set(GMOCK_STATIC_LIB ${INSTALL_DIR}/lib/libgmock.a)
    endif()

    add_library(gtest STATIC IMPORTED GLOBAL)
    set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${GTEST_STATIC_LIB})
    add_dependencies(gtest googletest)
    add_library(gmock STATIC IMPORTED GLOBAL)
    set_property(TARGET gmock PROPERTY IMPORTED_LOCATION ${GMOCK_STATIC_LIB})
    add_dependencies(gmock googletest)
endif()

#ExternalProject_Add(libev
#    URL http://dist.schmorp.de/libev/libev-4.33.tar.gz
#    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
#    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
#    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
#    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libev.so
#)
#ExternalProject_Get_Property(libev INSTALL_DIR)
#ExternalProject_Get_Property(libev SOURCE_DIR)
#add_library(ev STATIC IMPORTED GLOBAL)
#set_property(TARGET ev PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libev.dylib)
#add_dependencies(gtest googletest)
   

include_directories(${INSTALL_DIR}/include)

