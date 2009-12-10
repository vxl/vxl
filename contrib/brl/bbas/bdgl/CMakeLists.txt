# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bdgl
  DEPENDS contrib/gel/vtol contrib/oxl/mvl contrib/mul/mbl core/vbl
  PACKAGE contrib/brl
)

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

SET(bdgl_sources
    dll.h

    bdgl_curve_algs.cxx   bdgl_curve_algs.h
    bdgl_region_algs.cxx  bdgl_region_algs.h
    bdgl_curve_region.cxx bdgl_curve_region.h
    bdgl_peano_curve.cxx  bdgl_peano_curve.h
)

ADD_LIBRARY(bdgl ${bdgl_sources})
INSTALL_TARGETS(/lib bdgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bdgl ${bdgl_sources})

TARGET_LINK_LIBRARIES(bdgl vdgl vsol vgl_algo vgl vnl_algo vnl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
