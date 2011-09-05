# contrib/brl/bseg/bvpl/bvpl_octree/sample/CMakeLists.txt

SET(bvpl_octree_sample_sources
    bvpl_octree_sample.h           bvpl_octree_sample.txx
    bvpl_taylor_basis_sample.h     bvpl_taylor_basis_sample.cxx
    bvpl_pca_basis_sample.h        bvpl_pca_basis_sample.txx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_sample_sources)

ADD_LIBRARY(bvpl_octree_sample ${bvpl_octree_sample_sources})
TARGET_LINK_LIBRARIES(bvpl_octree_sample vnl_io vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
