INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bsta)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )

IF (WIN32 AND VXL_USE_LFS)
#This is a hack since win32 doesn't have transparent Large File Support.
  ADD_DEFINITIONS( -DBVXM_USE_FSTREAM64 )
  # so we can include vil_stream_fstream64
  ADD_DEFINITIONS( -DVIL_USE_FSTREAM64 )
ENDIF (WIN32 AND VXL_USE_LFS)

SET(bvxm_sources

    bvxm_memory_chunk.h             bvxm_memory_chunk.cxx 
    bvxm_mog_grey_processor.h       bvxm_mog_grey_processor.cxx  
    bvxm_mog_rgb_processor.h        bvxm_mog_rgb_processor.cxx
    bvxm_lidar_processor.h          bvxm_lidar_processor.cxx
    bvxm_world_params.h             bvxm_world_params.cxx
    bvxm_voxel_slab_base.h
    bvxm_voxel_slab.h               bvxm_voxel_slab.txx
    bvxm_voxel_storage.h           
    bvxm_voxel_storage_disk.h       bvxm_voxel_storage_disk.txx
    bvxm_voxel_storage_mem.h        bvxm_voxel_storage_mem.txx
    bvxm_voxel_slab_iterator.h      bvxm_voxel_slab_iterator.txx
    bvxm_voxel_grid_base.h          
    bvxm_voxel_grid.h               bvxm_voxel_grid.txx
    bvxm_voxel_traits.h
    bvxm_voxel_world.h              bvxm_voxel_world.cxx
    bvxm_image_metadata.h           bvxm_image_metadata.cxx
    bvxm_util.h                     bvxm_util.cxx  
)

INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )
    SET(bvxm_sources ${bvxm_sources}
      bvxm_lidar_camera.h                    bvxm_lidar_camera.cxx
    )
  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)
  
AUX_SOURCE_DIRECTORY(Templates bvxm_sources)

ADD_LIBRARY( bvxm ${bvxm_sources})


TARGET_LINK_LIBRARIES( bvxm bsta bsta_algo vcl vgl vgl_io vnl vil vgl_algo vil_algo vpgl vul vbl vsl bgeo brip)

SUBDIRS( io )
SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



