# brl/CMakeLists.txt
# Mutual dependencies:  bbas --> bseg --> vvid  and  bbas --> bmvl

PROJECT( brl )

doxygen_add_package(contrib/brl "Brown University Libraries")

# In contrib/brl there is sometimes an "extra" level in the directory structure
# (.../contrib/brl/{bbas,bseg,bmvl}/foo/foo.h)
# Setting BRL_INCLUDE_DIR in this way allows #include <foo/foo.h> without
# having to know the internal directory structure.
# The root directory should be listed last in case anyone tries to append to this path
SET( BRL_INCLUDE_DIR "${brl_SOURCE_DIR}/bbas;${brl_BINARY_DIR}/bbas;${brl_SOURCE_DIR}/bseg;${brl_SOURCE_DIR}/bmvl;${brl_SOURCE_DIR}/bpro;${brl_SOURCE_DIR}" CACHE STATIC "Brown University package include files" FORCE )
SET( BRL_BINARY_DIR "${brl_BINARY_DIR}")
SET( BRL_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bvml
  CACHE INTERNAL "Brown University package include files"
)

SET( BRL_MODULE_PATH ${vxl_SOURCE_DIR}/contrib/brl/bmods )


INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

# basic services package
SUBDIRS(bbas)

IF( BUILD_GEL )
  # segmentation package
  SUBDIRS(bseg)

  # video view library
  SUBDIRS(vvid)
ENDIF( BUILD_GEL )

# multiview library
SUBDIRS(bmvl)

# standalone executables
SUBDIRS(bexe)

# process libraries
SUBDIRS(bpro)

#3rd party libraries
SUBDIRS(b3p)
