project( CppCrate )

add_definitions( -DCPPCRATE_LIBRARY -DRAPIDJSON_HAS_STDSTRING=1 )

include_directories( ${CPPCRATE_INCLUDE_DIRS}
                     ${CURL_INCLUDE_DIRS}
                     ${RAPIDJSON_INCLUDE_DIRS} )

set( HEADERS_PUBLIC  ${CPPCRATE_INCLUDE_DIRS}/cppcrate/global.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/client.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/node.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/rawresult.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/result.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/value.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/cratedatatype.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/query.h
                     ${CPPCRATE_INCLUDE_DIRS}/cppcrate/record.h )

set( SOURCES_IMPL    client.cpp
                     node.cpp
                     rawresult.cpp
                     result.cpp
                     value.cpp
                     cratedatatype.cpp
                     query.cpp
                     record.cpp )

if( ENABLE_BLOB_SUPPORT )
    list( APPEND HEADERS_PUBLIC ${CPPCRATE_INCLUDE_DIRS}/cppcrate/blobresult.h )
    list( APPEND SOURCES_IMPL   crypto.h
                                crypto.cpp
                                blobresult.cpp
                                ${SHA1_INCLUDE_DIRS}/sha1/sha1.hpp
                                ${SHA1_INCLUDE_DIRS}/sha1/sha1.cpp )
    include_directories( ${SHA1_INCLUDE_DIRS} )
endif()

add_library( ${CPPCRATE_LIBRARIES} SHARED ${HEADERS_PUBLIC} ${SOURCES_IMPL} )

target_link_libraries( ${CPPCRATE_LIBRARIES} ${CURL_LIBRARIES} )

if( ENABLE_CPP11_SUPPORT )
    if( CMAKE_COMPILER_IS_GNUCC )
        target_compile_options( ${CPPCRATE_LIBRARIES} INTERFACE -std=c++11 )
    endif()
else()
    if( CMAKE_COMPILER_IS_GNUCC )
        target_compile_options( ${CPPCRATE_LIBRARIES} INTERFACE -std=c++98 )
    endif()
endif()