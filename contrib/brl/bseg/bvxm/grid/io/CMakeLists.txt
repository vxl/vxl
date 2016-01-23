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

add_library(bvxm_grid_io ${bvxm_grid_io_sources})
target_link_libraries(bvxm_grid_io bvxm_grid bvrml vsl vbl vil vil_io vnl vgl vbl_io vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
