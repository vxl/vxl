# bpgl/algo/CMakeLists.txt
# Photogrammetry Algorithms Library.

include_directories( ${GEL_INCLUDE_DIR} )

set( bpgl_algo_sources
  bpgl_construct_cameras.h         bpgl_construct_cameras.cxx
  bpgl_vsol_lens_warp.h            bpgl_vsol_lens_warp.cxx
  bpgl_list.h                      bpgl_list.hxx
  bpgl_interpolate.h               bpgl_interpolate.cxx
  bpgl_camera_homographies.h       bpgl_camera_homographies.cxx
  bpgl_nitf_camera_coverage.h      bpgl_nitf_camera_coverage.cxx
  bpgl_camera_from_box.h           bpgl_camera_from_box.cxx
  bpgl_bundle_rolling_shutter_adjust.h  bpgl_bundle_rolling_shutter_adjust.cxx
  bpgl_project.h                   bpgl_project.cxx
  bpgl_transform_camera.h                   bpgl_transform_camera.cxx
  bpgl_gridding.h
  bpgl_3d_from_disparity.h  bpgl_3d_from_disparity.hxx
  bpgl_heightmap_from_disparity.h  bpgl_heightmap_from_disparity.hxx
)

# the ransac implementations use rpl/rrel
if(VXL_BUILD_RPL)
  include_directories(${RPL_INCLUDE_DIR})
  set(bpgl_algo_sources ${bpgl_algo_sources}
      bpgl_fm_compute_ransac.h         bpgl_fm_compute_ransac.cxx
      bpgl_fm_compute_reg_ransac.h     bpgl_fm_compute_reg_ransac.cxx
      bpgl_fm_compute_affine_ransac.h  bpgl_fm_compute_affine_ransac.cxx
     )
endif()


aux_source_directory(Templates bpgl_algo_sources)

vxl_add_library(LIBRARY_NAME bpgl_algo LIBRARY_SOURCES ${bpgl_algo_sources})

target_link_libraries(bpgl_algo bpgl bvgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_file_formats vsol ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)

if(VXL_BUILD_RPL)
  target_link_libraries(bpgl_algo rrel)
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if( VXL_BUILD_EXAMPLES )
#  add_subdirectory(examples)
endif()
