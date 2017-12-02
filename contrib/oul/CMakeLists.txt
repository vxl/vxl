# ./oul/CMakeLists.txt

project( oul )

doxygen_add_package(contrib/oul "Otago University Libraries")

set( OUL_INCLUDE_DIR ${oul_SOURCE_DIR} CACHE INTERNAL "Otago University package include files" )
set( OUL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/oul
  CACHE INTERNAL "Otago University package include files"
)

include_directories( ${OUL_INCLUDE_DIR} )

set(oul_sources oul_copyright.h)
INSTALL_NOBASE_HEADER_FILES(include/vxl/contrib/oul ${oul_sources})

add_subdirectory(ouel)
add_subdirectory(ouml)
# V4L has been removed in 2.6.38
CHECK_INCLUDE_FILE("linux/videodev.h" OUL_HAVE_LINUX_VIDEODEV_H)
if(OUL_HAVE_LINUX_VIDEODEV_H)
if(CMAKE_SYSTEM MATCHES "Linux*")
  add_subdirectory(oufgl)
endif()
endif()
