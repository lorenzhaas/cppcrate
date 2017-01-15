macro( add_custom_test CUSTOM_TEST_NAME )
    add_executable( ${CUSTOM_TEST_NAME}_tests ${CUSTOM_TEST_NAME}_tests.cpp )
    target_link_libraries( ${CUSTOM_TEST_NAME}_tests ${GTEST_LIBRARIES} ${CPPCRATE_LIBRARIES} )
    add_test( NAME cppcrate_${CUSTOM_TEST_NAME}_tests COMMAND ${CUSTOM_TEST_NAME}_tests )
endmacro()

include_directories( ${CPPCRATE_INCLUDE_DIRS}
                     ${CURL_INCLUDE_DIRS}
                     ${GTEST_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

add_custom_test( node )
add_custom_test( rawresult )
add_custom_test( cratedatatype )
add_custom_test( query )
if( ENABLE_BLOB_SUPPORT )
    add_custom_test( blobresult )
endif()
