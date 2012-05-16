# This is contrib/brl/bseg/bvpl/util/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )

SET(bvpl_util_sources
    bvpl_corner_pair_finder.h    bvpl_corner_pair_finder.cxx
    bvpl_corner_search_functor.h bvpl_corner_search_functor.cxx
    bvpl_bundler_features_2d.h   bvpl_bundler_features_2d.cxx
   )
AUX_SOURCE_DIRECTORY(Templates bvpl_util_sources)

ADD_LIBRARY(bvpl_util ${bvpl_util_sources})
TARGET_LINK_LIBRARIES(bvpl_util bvpl brdb vil_algo vgl_io vgl vnl_io vnl vbl)

SUBDIRS(io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
