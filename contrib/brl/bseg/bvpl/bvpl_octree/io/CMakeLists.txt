# contrib/brl/bseg/bvpl/io/CMakeLists.txt

SET(bvpl_octree_io_sources
    bvpl_io_taylor_scenes_map.h       bvpl_io_taylor_scenes_map.cxx
    bvpl_io_pca_error_scenes.h        bvpl_io_pca_error_scenes.cxx
    bvpl_io_discover_pca_kernels.h    bvpl_io_discover_pca_kernels.cxx
    bvpl_io_global_pca.h              bvpl_io_global_pca.cxx
    bvpl_io_global_taylor.h          
    bvpl_io_global_corners.h          bvpl_io_global_corners.cxx

)

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_io_sources)

ADD_LIBRARY(bvpl_octree_io ${bvpl_octree_io_sources})
TARGET_LINK_LIBRARIES(bvpl_octree_io bvpl_octree vbl_io vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
