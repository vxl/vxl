# contrib/brl/bseg/bvxm/grid/io/CMakeLists.txt

set(bvxm_grid_io_sources
    bvxm_io_voxel_slab_base.h  bvxm_io_voxel_slab_base.cxx
    bvxm_io_voxel_grid_base.h  bvxm_io_voxel_grid_base.cxx
    bvxm_io_voxel_grid.h       bvxm_io_voxel_grid.cxx
    bvxm_slab_to_image.h       bvxm_slab_to_image.cxx
    bvxm_grid_to_image_stack.h bvxm_grid_to_image_stack.cxx
    bvxm_vrml_voxel_grid.h     bvxm_vrml_voxel_grid.cxx
)

aux_source_directory(Templates bvxm_grid_io_sources)

vxl_add_library(LIBRARY_NAME bvxm_grid_io LIBRARY_SOURCES ${bvxm_grid_io_sources})
target_link_libraries(bvxm_grid_io bvxm_grid bvrml ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
