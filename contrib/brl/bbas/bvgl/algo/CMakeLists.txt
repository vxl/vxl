# bbas/bvgl/algo/CMakeLists.txt

PROJECT(bvgl_algo)

SET( bvgl_algo_sources
  bvgl_2d_geo_index_node_base.h     bvgl_2d_geo_index_node_base.cxx
  bvgl_2d_geo_index.h               bvgl_2d_geo_index.cxx
  bvgl_2d_geo_index_sptr.h
  bvgl_2d_geo_index.txx
)
AUX_SOURCE_DIRECTORY(Templates bvgl_algo_sources)

ADD_LIBRARY(bvgl_algo ${bvgl_algo_sources})
TARGET_LINK_LIBRARIES(bvgl_algo bvgl bkml vpgl vgl vbl vbl_io vsl )

INSTALL_TARGETS(/lib bvgl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvgl/algo ${bvgl_algo_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )