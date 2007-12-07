# Brown basic geometry library
# brl/bbas/bvgl/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(bvgl_sources
 bvgl_articulated_poly.h bvgl_articulated_poly.cxx bvgl_articulated_poly_sptr.h
)
AUX_SOURCE_DIRECTORY(Templates bvgl_sources)

ADD_LIBRARY(bvgl ${bvgl_sources})
INSTALL_TARGETS(/lib bvgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvgl ${bvgl_sources})
TARGET_LINK_LIBRARIES(bvgl vgl vsol vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
