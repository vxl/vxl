include( ${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
endif()
if(GEOTIFF_FOUND)
include_directories(${GEOTIFF_INCLUDE_DIR})

include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas)
include_directories( ${BRL_INCLUDE_DIR}/bseg )

if(WIN32 AND VXL_USE_LFS)
#This is a hack since win32 doesn't have transparent Large File Support.
add_definitions( -DBVXM_USE_FSTREAM64 )
  # so we can include vil_stream_fstream64
add_definitions( -DVIL_USE_FSTREAM64 )
endif()

set(bvxm_sources
    bvxm_float_processor.h
    bvxm_mog_grey_processor.h       bvxm_mog_grey_processor.cxx
    bvxm_mog_rgb_processor.h        bvxm_mog_rgb_processor.cxx
    bvxm_mog_mc_processor.h         bvxm_mog_mc_processor.hxx
    bvxm_lidar_processor.h          bvxm_lidar_processor.cxx
    bvxm_world_params.h             bvxm_world_params.cxx
    bvxm_voxel_traits.h
    bvxm_voxel_world.h              bvxm_voxel_world.cxx
    bvxm_image_metadata.h           bvxm_image_metadata.cxx
    bvxm_util.h                     bvxm_util.cxx
    bvxm_edge_ray_processor.h       bvxm_edge_ray_processor.cxx
    bvxm_von_mises_tangent_processor.h     bvxm_von_mises_tangent_processor.hxx
    bvxm_illum_util.h                     bvxm_illum_util.cxx
    bvxm_edge_util.h                      bvxm_edge_util.cxx
   )
aux_source_directory(Templates bvxm_sources)

#find expatpp library for bvxm_world_params parser
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )
  set(bvxm_sources ${bvxm_sources}
      bvxm_world_params_parser.h             bvxm_world_params_parser.cxx
     )
endif()

vxl_add_library(LIBRARY_NAME bvxm LIBRARY_SOURCES ${bvxm_sources})

target_link_libraries( bvxm bvxm_grid bsta bsta_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_file_formats brip sdet bil_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

if(EXPAT_FOUND)
target_link_libraries(bvxm expatpp)
endif()

add_subdirectory(grid)
add_subdirectory(io)
add_subdirectory(pro)
add_subdirectory(algo)
add_subdirectory(breg3d)
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/bvxm: requires geotiff")
endif()
