include_directories( ${MUL_INCLUDE_DIR} )

if(WIN32 AND VXL_USE_LFS)
#This is a hack since win32 doesn't have transparent Large File Support.
add_definitions( -DBVXM_USE_FSTREAM64 )
  # so we can include vil_stream_fstream64
add_definitions( -DVIL_USE_FSTREAM64 )
endif()

set(bvxm_grid_sources
    bvxm_memory_chunk.h               bvxm_memory_chunk.cxx
    bvxm_voxel_slab_base.h
    bvxm_voxel_slab.h                 bvxm_voxel_slab.hxx
    bvxm_voxel_storage.h
    bvxm_voxel_storage_disk.h         bvxm_voxel_storage_disk.hxx
    bvxm_voxel_storage_disk_cached.h  bvxm_voxel_storage_disk_cached.hxx
    bvxm_voxel_storage_mem.h          bvxm_voxel_storage_mem.hxx
    bvxm_voxel_storage_slab_mem.h     bvxm_voxel_storage_slab_mem.hxx
    bvxm_voxel_slab_iterator.h        bvxm_voxel_slab_iterator.hxx
    bvxm_voxel_grid_base.h            bvxm_voxel_grid_base_sptr.h
    bvxm_voxel_grid.h                 bvxm_voxel_grid.hxx
    bvxm_voxel_grid_basic_ops.h       bvxm_voxel_grid_basic_ops.cxx
    bvxm_opinion.h
    bvxm_voxel_grid_opinion_basic_ops.h bvxm_voxel_grid_opinion_basic_ops.cxx
)

aux_source_directory(Templates bvxm_grid_sources)

vxl_add_library(LIBRARY_NAME bvxm_grid LIBRARY_SOURCES ${bvxm_grid_sources})

target_link_libraries( bvxm_grid ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo vil3d vil3d_algo ${VXL_LIB_PREFIX}vcl)

add_subdirectory(io)
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
