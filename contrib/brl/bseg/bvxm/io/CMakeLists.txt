# contrib/brl/bseg/bvxm/io/CMakeLists.txt

set(bvxm_io_sources
    bvxm_io_voxel_world.h  bvxm_io_voxel_world.cxx
    bvxm_io_world_params.h bvxm_io_world_params.cxx
   )

aux_source_directory(Templates bvxm_io_sources)

vxl_add_library(LIBRARY_NAME bvxm_io LIBRARY_SOURCES ${bvxm_io_sources})
target_link_libraries(bvxm_io bvxm ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
