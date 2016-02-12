# contrib/brl/bseg/bvpl/bvpl_octree/sample/CMakeLists.txt

set(bvpl_octree_sample_sources
    bvpl_octree_sample.h           bvpl_octree_sample.txx
    bvpl_taylor_basis_sample.h     bvpl_taylor_basis_sample.cxx
    bvpl_pca_basis_sample.h        bvpl_pca_basis_sample.txx
   )

aux_source_directory(Templates bvpl_octree_sample_sources)

add_library(bvpl_octree_sample ${bvpl_octree_sample_sources})
target_link_libraries(bvpl_octree_sample ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
