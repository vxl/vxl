# bpgl/algo/CMakeLists.txt
# Photogrammetry Algorithms Library.

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

SET( bpgl_algo_sources
  bpgl_construct_cameras.h         bpgl_construct_cameras.cxx
  bpgl_lens_warp_mapper.h
  bpgl_vsol_lens_warp.h            bpgl_vsol_lens_warp.cxx
  bpgl_list.h                      bpgl_list.txx
  bpgl_interpolate.h               bpgl_interpolate.cxx
  bpgl_camera_homographies.h       bpgl_camera_homographies.cxx
  bpgl_nitf_camera_coverage.h      bpgl_nitf_camera_coverage.cxx
  bpgl_camera_from_box.h           bpgl_camera_from_box.cxx
  bpgl_bundle_rolling_shutter_adjust.h  bpgl_bundle_rolling_shutter_adjust.cxx
  bpgl_project.h                   bpgl_project.cxx
  bpgl_transform_camera.h		   bpgl_transform_camera.cxx
)

# the ransac implementations use rpl/rrel
IF(BUILD_RPL)
  INCLUDE_DIRECTORIES(${RPL_INCLUDE_DIR})
  SET(bpgl_algo_sources ${bpgl_algo_sources}
      bpgl_fm_compute_ransac.h         bpgl_fm_compute_ransac.cxx
      bpgl_fm_compute_reg_ransac.h     bpgl_fm_compute_reg_ransac.cxx
      bpgl_fm_compute_affine_ransac.h  bpgl_fm_compute_affine_ransac.cxx
     )
ENDIF(BUILD_RPL)


AUX_SOURCE_DIRECTORY(Templates bpgl_algo_sources)

ADD_LIBRARY(bpgl_algo ${bpgl_algo_sources})

TARGET_LINK_LIBRARIES(bpgl_algo bpgl vpgl vpgl_file_formats vsol vgl_algo vnl_algo vnl vgl vil vul vbl)

IF(BUILD_RPL)
  TARGET_LINK_LIBRARIES(bpgl_algo rrel)
ENDIF(BUILD_RPL)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

INSTALL_TARGETS(/lib bpgl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bpgl/algo ${bpgl_algo_sources})
