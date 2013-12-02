INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

IF (WIN32 AND VXL_USE_LFS)
#This is a hack since win32 doesn't have transparent Large File Support.
ADD_DEFINITIONS( -DBVXM_USE_FSTREAM64 )
  # so we can include vil_stream_fstream64
ADD_DEFINITIONS( -DVIL_USE_FSTREAM64 )
ENDIF (WIN32 AND VXL_USE_LFS)

SET(bvxm_grid_sources
    bvxm_memory_chunk.h               bvxm_memory_chunk.cxx
    bvxm_voxel_slab_base.h
    bvxm_voxel_slab.h                 bvxm_voxel_slab.txx
    bvxm_voxel_storage.h
    bvxm_voxel_storage_disk.h         bvxm_voxel_storage_disk.txx
    bvxm_voxel_storage_disk_cached.h  bvxm_voxel_storage_disk_cached.txx
    bvxm_voxel_storage_mem.h          bvxm_voxel_storage_mem.txx
    bvxm_voxel_storage_slab_mem.h     bvxm_voxel_storage_slab_mem.txx
    bvxm_voxel_slab_iterator.h        bvxm_voxel_slab_iterator.txx
    bvxm_voxel_grid_base.h            bvxm_voxel_grid_base_sptr.h
    bvxm_voxel_grid.h                 bvxm_voxel_grid.txx
    bvxm_voxel_grid_basic_ops.h       bvxm_voxel_grid_basic_ops.cxx
    bvxm_opinion.h
    bvxm_voxel_grid_opinion_basic_ops.h bvxm_voxel_grid_opinion_basic_ops.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvxm_grid_sources)

ADD_LIBRARY( bvxm_grid ${bvxm_grid_sources})

TARGET_LINK_LIBRARIES( bvxm_grid vpgl vnl vil vil_algo vul vbl vsl vgl vgl_algo vil3d vil3d_algo vcl)

SUBDIRS( io )
SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
