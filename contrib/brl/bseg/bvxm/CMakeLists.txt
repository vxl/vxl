INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE( ${VXL_CMAKE_DIR}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)

IF (WIN32 AND VXL_USE_LFS)
#This is a hack since win32 doesn't have transparent Large File Support.
ADD_DEFINITIONS( -DBVXM_USE_FSTREAM64 )
  # so we can include vil_stream_fstream64
ADD_DEFINITIONS( -DVIL_USE_FSTREAM64 )
ENDIF (WIN32 AND VXL_USE_LFS)

SET(bvxm_sources
    bvxm_float_processor.h
    bvxm_mog_grey_processor.h       bvxm_mog_grey_processor.cxx
    bvxm_mog_rgb_processor.h        bvxm_mog_rgb_processor.cxx
    bvxm_mog_mc_processor.h         bvxm_mog_mc_processor.txx
    bvxm_lidar_processor.h          bvxm_lidar_processor.cxx
    bvxm_world_params.h             bvxm_world_params.cxx
    bvxm_voxel_traits.h
    bvxm_voxel_world.h              bvxm_voxel_world.cxx
    bvxm_image_metadata.h           bvxm_image_metadata.cxx
    bvxm_util.h                     bvxm_util.cxx
    bvxm_edge_ray_processor.h       bvxm_edge_ray_processor.cxx
    bvxm_von_mises_tangent_processor.h     bvxm_von_mises_tangent_processor.txx
    bvxm_illum_util.h                     bvxm_illum_util.cxx
    bvxm_edge_util.h                      bvxm_edge_util.cxx
   )
AUX_SOURCE_DIRECTORY(Templates bvxm_sources)

ADD_LIBRARY( bvxm ${bvxm_sources})

TARGET_LINK_LIBRARIES( bvxm bvxm_grid bsta bsta_algo vpgl vpgl_io vpgl_file_formats brip sdet bil_algo vgl_io vgl_algo vil_algo vnl_algo vnl vgl vil vul vbl vsl vcl)

SUBDIRS( grid )
SUBDIRS( io )
SUBDIRS( pro )
SUBDIRS( algo )
SUBDIRS( breg3d )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
