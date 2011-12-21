# ./oul/CMakeLists.txt

PROJECT( oul )

doxygen_add_package(contrib/oul "Otago University Libraries")

SET( OUL_INCLUDE_DIR ${oul_SOURCE_DIR} CACHE INTERNAL "Otago University package include files" )
SET( OUL_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/oul
  CACHE INTERNAL "Otago University package include files"
)

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OUL_INCLUDE_DIR} )

SET(oul_sources oul_copyright.h)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul ${oul_sources})

SUBDIRS(ouel ouml)
# V4L has been removed in 2.6.38
CHECK_INCLUDE_FILE("linux/videodev.h" OUL_HAVE_LINUX_VIDEODEV_H)
IF(OUL_HAVE_LINUX_VIDEODEV_H)
IF(CMAKE_SYSTEM MATCHES "Linux*")
  SUBDIRS(oufgl)
ENDIF(CMAKE_SYSTEM MATCHES "Linux*")
ENDIF(OUL_HAVE_LINUX_VIDEODEV_H)
