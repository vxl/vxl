# contrib/brl/bseg/bvpl/io/CMakeLists.txt

SET(bvpl_octree_io_sources
    bvpl_io_taylor_scenes_map.h       bvpl_io_taylor_scenes_map.cxx
    bvpl_io_pca_error_scenes.h        bvpl_io_pca_error_scenes.cxx
    bvpl_io_discover_pca_kernels.h    bvpl_io_discover_pca_kernels.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_io_sources)

ADD_LIBRARY(bvpl_octree_io ${bvpl_octree_io_sources})
TARGET_LINK_LIBRARIES(bvpl_octree_io bvpl_octree vbl_io vsl vbl)
