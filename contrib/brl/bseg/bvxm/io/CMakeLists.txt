# contrib/brl/bseg/bvxm/io/CMakeLists.txt

SET(bvxm_io_sources
    bvxm_io_voxel_world.h  bvxm_io_voxel_world.cxx
    bvxm_io_world_params.h bvxm_io_world_params.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvxm_io_sources)

ADD_LIBRARY(bvxm_io ${bvxm_io_sources})
TARGET_LINK_LIBRARIES(bvxm_io bvxm vsl vbl vil vil_io)


