# contrib/brl/bseg/bvpl/io/CMakeLists.txt

set(bvpl_octree_io_sources
    bvpl_io_taylor_scenes_map.h       bvpl_io_taylor_scenes_map.cxx
    bvpl_io_pca_error_scenes.h        bvpl_io_pca_error_scenes.cxx
    bvpl_io_discover_pca_kernels.h    bvpl_io_discover_pca_kernels.cxx
    bvpl_io_global_pca.h              bvpl_io_global_pca.cxx
    bvpl_io_global_taylor.h
    bvpl_io_global_corners.h          bvpl_io_global_corners.cxx

)

aux_source_directory(Templates bvpl_octree_io_sources)

vxl_add_library(LIBRARY_NAME bvpl_octree_io LIBRARY_SOURCES ${bvpl_octree_io_sources})
target_link_libraries(bvpl_octree_io bvpl_octree ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
