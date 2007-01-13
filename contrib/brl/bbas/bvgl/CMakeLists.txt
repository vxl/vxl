# Brown basic geometry library
# brl/bbas/bvgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bvgl_sources
bvgl_articulated_poly.cxx   bvgl_articulated_poly.h
)
AUX_SOURCE_DIRECTORY(Templates bvgl_sources)

ADD_LIBRARY(bvgl ${bvgl_sources})
INSTALL_TARGETS(/lib bvgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvgl ${bvgl_sources})
TARGET_LINK_LIBRARIES(bvgl vgl vnl_algo vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
