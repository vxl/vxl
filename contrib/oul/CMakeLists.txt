# ./oul/CMakeLists.txt

PROJECT( oul )

SET( OUL_INCLUDE_DIR ${oul_SOURCE_DIR} CACHE STATIC "Otago University package include files" )
SET( OUL_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/contrib/oul
  CACHE STATIC "Otago University package include files"
)

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OUL_INCLUDE_DIR} )

SUBDIRS(ouel ouml)
IF(CMAKE_SYSTEM MATCHES "Linux*")
  SUBDIRS(oufgl)
ENDIF(CMAKE_SYSTEM MATCHES "Linux*")
