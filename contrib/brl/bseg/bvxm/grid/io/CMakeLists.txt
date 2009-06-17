# contrib/brl/bseg/bvxm/grid/io/CMakeLists.txt

SET(bvxm_grid_io_sources
    bvxm_io_voxel_slab_base.h  bvxm_io_voxel_slab_base.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvxm_grid_io_sources)

ADD_LIBRARY(bvxm_grid_io ${bvxm_grid_io_sources})
TARGET_LINK_LIBRARIES(bvxm_grid_io bvxm_grid vsl vbl vil vil_io)


