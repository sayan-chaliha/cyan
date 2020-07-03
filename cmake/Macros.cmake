# Install headers macro
MACRO(install_headers HEADER_LIST)
    foreach(HEADER ${HEADER_LIST})
        get_filename_component(DIR ${HEADER} DIRECTORY)
        install(FILES ${HEADER} DESTINATION include/${DIR})
    ENDFOREACH(HEADER)
ENDMACRO(install_headers)
