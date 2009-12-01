# This is contrib/brl/bseg/bvpl/bvpl_octree/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boct )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )


SET(bvpl_octree_sources
    bvpl_octree_kernel_operator.h
    bvpl_octree_vector_operator.h 
    bvpl_scene_kernel_operator.h 
    bvpl_scene_vector_operator.h
    temp.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_sources)

ADD_LIBRARY(bvpl_octree ${bvpl_octree_sources})

TARGET_LINK_LIBRARIES(bvpl_octree bvpl boct boxm)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )