# This is contrib/brl/bseg/bvpl/util/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )


SET(bvpl_util_sources
    #bvpl_corner_search_kernel.h bvpl_corner_search_kernel.cxx
    bvpl_corner_pair_finder.h    bvpl_corner_pair_finder.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_util_sources)

ADD_LIBRARY(bvpl_util ${bvpl_util_sources})

TARGET_LINK_LIBRARIES(bvpl_util bvpl brdb)

SUBDIRS(io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
