# This is contrib/brl/bseg/bvpl/bvpl_octree/CMakeLists.txt
# Brown Voxel Processing library for octrees

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

#OPTION( VXL_FORCE_V3P_RPLY "Use v3p ply library" YES)
#MARK_AS_ADVANCED( VXL_FORCE_V3P_RPLY )

IF(EXPAT_FOUND)

FIND_PACKAGE( Threads )

#CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/bvpl_octree_config.h.in
#                ${CMAKE_CURRENT_BINARY_DIR}/bvpl_octree_config.h )

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
    bvpl_block_kernel_operator.h
    bvpl_block_vector_operator.h
    bvpl_octree_vrml_util.h
    bvpl_cell_data_traits.h
    bvpl_octree_neighbors.h
    bvpl_scene_neighborhood_operator.h    bvpl_scene_neighborhood_operator.cxx
    bvpl_scene_statistics.h               bvpl_scene_statistics.cxx
    bvpl_discover_pca_kernels.h           bvpl_discover_pca_kernels.cxx
    bvpl_pca_error_scenes.h               bvpl_pca_error_scenes.cxx
    bvpl_global_pca.h                     bvpl_global_pca.txx
    bvpl_taylor_scenes_map.h              bvpl_taylor_scenes_map.cxx
    bvpl_taylor_basis.h                   bvpl_taylor_basis.cxx
    bvpl_global_taylor.h                  bvpl_global_taylor.txx          bvpl_global_taylor.cxx
    bvpl_global_corners.h                 bvpl_global_corners.cxx
    bvpl_corner_detector.h
    bvpl_corner_functors.h
    bvpl_gauss3D_steerable_filters.h      bvpl_gauss3D_steerable_filters.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_sources)

ADD_LIBRARY(bvpl_octree ${bvpl_octree_sources})
TARGET_LINK_LIBRARIES(bvpl_octree bvpl_kernels bvpl_functors bvpl_octree_sample bvrml boxm boct vnl_algo vnl_io vnl vgl vul vbl)

#Binary io
SUBDIRS(io)

#Directory with python/c++ processes
SUBDIRS(pro)

# Octree samples needed by bvpl
SUBDIRS(sample)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF( CMAKE_USE_PTHREADS_INIT )
  ADD_DEFINITIONS(-DBVPL_OCTREE_HAS_PTHREADS=1)
  SET_TARGET_PROPERTIES(bvpl_octree PROPERTIES COMPILE_FLAGS -pthread)
  TARGET_LINK_LIBRARIES(bvpl_octree pthread)
ENDIF( CMAKE_USE_PTHREADS_INIT )

ENDIF(EXPAT_FOUND)
