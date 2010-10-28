# This is contrib/brl/bseg/bvpl/bvpl_octree/CMakeLists.txt
# Brown Voxel Processing library for octrees

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boct )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(bvpl_octree_sources
    bvpl_octree_kernel_operator.h
    bvpl_octree_vector_operator.h
    bvpl_scene_kernel_operator.h
    bvpl_scene_vector_operator.h
    bvpl_octree_sample.h               bvpl_octree_sample.txx
    bvpl_octree_vrml_util.h
    bvpl_cell_data_traits.h
    bvpl_octree_neighbors.h
    bvpl_scene_neighborhood_operator.h bvpl_scene_neighborhood_operator.cxx
    bvpl_scene_statistics.h           bvpl_scene_statistics.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_sources)

ADD_LIBRARY(bvpl_octree ${bvpl_octree_sources})

TARGET_LINK_LIBRARIES(bvpl_octree bvpl bvrml boxm boct vgl vbl)

#Directory with python/c++ processes
SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
