# This is contrib/brl/bseg/bvpl/bvpl_octree/CMakeLists.txt
# Brown Voxel Processing library for octrees

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

#option( VXL_FORCE_V3P_RPLY "Use v3p ply library" YES)
#mark_as_advanced( VXL_FORCE_V3P_RPLY )

if(EXPAT_FOUND)

find_package( Threads )

#vxl_configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/bvpl_octree_config.h.in
#                ${CMAKE_CURRENT_BINARY_DIR}/bvpl_octree_config.h include/vxl/contrib/brl/bseg/bvpl/bvpl_octree)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boct )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(bvpl_octree_sources
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
    bvpl_global_pca.h                     bvpl_global_pca.hxx
    bvpl_taylor_scenes_map.h              bvpl_taylor_scenes_map.cxx
    bvpl_taylor_basis.h                   bvpl_taylor_basis.cxx
    bvpl_global_taylor.h                  bvpl_global_taylor.hxx          bvpl_global_taylor.cxx
    bvpl_global_corners.h                 bvpl_global_corners.cxx
    bvpl_corner_detector.h
    bvpl_corner_functors.h
    bvpl_gauss3D_steerable_filters.h      bvpl_gauss3D_steerable_filters.cxx
   )

aux_source_directory(Templates bvpl_octree_sources)

vxl_add_library(LIBRARY_NAME bvpl_octree LIBRARY_SOURCES ${bvpl_octree_sources})
target_link_libraries(bvpl_octree bvpl_kernels bvpl_functors bvpl_octree_sample bvrml boxm boct ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)

#Binary io
add_subdirectory(io)

#Directory with python/c++ processes
add_subdirectory(pro)

# Octree samples needed by bvpl
add_subdirectory(sample)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if( CMAKE_USE_PTHREADS_INIT )
  add_definitions(-DBVPL_OCTREE_HAS_PTHREADS=1)
  set_target_properties(bvpl_octree PROPERTIES COMPILE_FLAGS -pthread)
  target_link_libraries(bvpl_octree pthread)
endif()

endif()
