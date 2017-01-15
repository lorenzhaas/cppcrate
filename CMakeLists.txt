cmake_minimum_required( VERSION 3.0 )

project( CppCrate )

set( CPPCRATE_VERSION_MAJOR 0 CACHE STRING "CppCrate major version number." )
set( CPPCRATE_VERSION_MINOR 1 CACHE STRING "CppCrate minor version number." )
set( CPPCRATE_LIBRARIES     cppcrate                                        )
set( CPPCRATE_INCLUDE_DIRS  ${CMAKE_CURRENT_SOURCE_DIR}/include             )



####################################################################################################
##                                                                                                ##
##  Define CMake Options                                                                          ##
##                                                                                                ##
####################################################################################################

macro( custom_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT )
    option( ${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT} )
    if( DEFINED ENV{${OPTION_NAME}} )
        set( ${OPTION_NAME} $ENV{${OPTION_NAME}} )
    endif()
    if( ${OPTION_NAME} )
        add_definitions( -D${OPTION_NAME} )
    endif()
    message( STATUS "    * ${OPTION_NAME}   ${${OPTION_NAME}}" )
endmacro()

message( STATUS "CppCrate options:" )
custom_option( ENABLE_BLOB_SUPPORT  "If ON, blob support will be included." ON  )
custom_option( ENABLE_CPP11_SUPPORT "If ON, C++11 fetures are used." ON  )
custom_option( BUILD_UNITTESTS      "If ON, the unit test will be build. (Needs ENABLE_CPP11_SUPPORT=ON)" OFF )



####################################################################################################
##                                                                                                ##
##  Include documentation                                                                         ##
##                                                                                                ##
####################################################################################################

add_subdirectory( doc )



####################################################################################################
##                                                                                                ##
##  Include curl                                                                                  ##
##                                                                                                ##
####################################################################################################

find_package( CURL )
if( NOT CURL_FOUND )
    message( FATAL_ERROR "Could not find curl." )
endif()



####################################################################################################
##                                                                                                ##
##  Include RapidJSON                                                                             ##
##                                                                                                ##
####################################################################################################

set( RAPIDJSON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty )



####################################################################################################
##                                                                                                ##
##  Include C++-SHA1                                                                              ##
##                                                                                                ##
####################################################################################################

set( SHA1_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty )



####################################################################################################
##                                                                                                ##
##  Include CppCrate                                                                              ##
##                                                                                                ##
####################################################################################################

add_subdirectory( src )



####################################################################################################
##                                                                                                ##
##  Include Google Test                                                                           ##
##                                                                                                ##
####################################################################################################

if( BUILD_UNITTESTS AND ENABLE_CPP11_SUPPORT )
    if( MSVC )
        set( gtest_force_shared_crt ON )
    endif()
    add_subdirectory( 3rdparty/googletest )
    set( GTEST_LIBRARIES      gtest )
    set( GTEST_INCLUDE_DIRS   ${gtest_SOURCE_DIR}/include )
    enable_testing()
    add_subdirectory( tests )
endif()
