cmake_minimum_required( VERSION 3.0 )

project( CppCrate )

find_package( Doxygen )
if( DOXYGEN_FOUND )
    set( MY_DOXYGEN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )
    set( MY_DOXYGEN_CONFIG_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/style/ghpages )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/style/ghpages/cppcrate.doxyfile.in
                    ${CMAKE_CURRENT_BINARY_DIR}/doc/ghpages/cppcrate.doxyfile @ONLY )
    add_custom_target( doc_ghpages
                       COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/ghpages/cppcrate.doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/ghpages
                       COMMENT "Generating CppCrate-API documentation"
                       VERBATIM )
else()
    message( WARNING "Doxygen not found. 'doc-***' targets will not be available." )
endif()
