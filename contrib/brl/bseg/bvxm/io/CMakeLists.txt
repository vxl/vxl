# contrib/brl/bseg/bvxm/io/CMakeLists.txt

set(bvxm_io_sources
    bvxm_io_voxel_world.h  bvxm_io_voxel_world.cxx
    bvxm_io_world_params.h bvxm_io_world_params.cxx
   )

aux_source_directory(Templates bvxm_io_sources)

add_library(bvxm_io ${bvxm_io_sources})
target_link_libraries(bvxm_io bvxm vsl vbl vil vil_io vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
