
cmake_minimum_required( VERSION 2.6 )
project( xrootd-davix )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

find_package( Xrootd REQUIRED )
find_package( Davix REQUIRED )

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror" )
endif()

if( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
endif()

SET( CMAKE_SHARED_LINKER_FLAGS "--no-undefined")
SET( CMAKE_MODULE_LINKER_FLAGS "--no-undefined")
SET( CMAKE_EXE_LINKER_FLAGS "--no-undefined")

include_directories( "${PROJECT_SOURCE_DIR}" "${XROOTD_INCLUDES}" "${DAVIX_INCLUDE_DIR}" )
add_definitions("${DAVIX_DEFINITIONS}")

add_library( XrdDavix MODULE src/XrdDavix.cc src/XrdDavixSys.cc src/Version.cc src/Config.cc )
target_link_libraries(XrdDavix ${XROOTD_UTILS} ${DAVIX_LIBRARY})

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR "lib")
endif()

install(
  TARGETS XrdDavix
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set_target_properties(
  XrdDavix
  PROPERTIES
  VERSION   "0.0.1"
  SOVERSION "0"
  LINK_INTERFACE_LIBRARIES "" )

