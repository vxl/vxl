# Brown view sphere directory
# brl/bbas/bpgl/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR} )

set(bpgl_sources
bpgl_comp_rational_camera.h         bpgl_comp_rational_camera.hxx
bpgl_reg_fundamental_matrix.h       bpgl_reg_fundamental_matrix.hxx
bpgl_rolling_shutter_camera.h
bpgl_segmented_rolling_shutter_camera.h bpgl_segmented_rolling_shutter_camera.hxx
bpgl_camera_estimator.h bpgl_camera_estimator.hxx bpgl_camera_estimator_amoeba.h
bpgl_camera_utils.h bpgl_camera_utils.cxx
   )

aux_source_directory(Templates bpgl_sources)

vxl_add_library(LIBRARY_NAME bpgl LIBRARY_SOURCES ${bpgl_sources})
target_link_libraries(bpgl brip ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vcl)

# algorithms involving cameras
add_subdirectory(algo)

# determine unknown camera parameters
add_subdirectory(icam)

# image homography registration
add_subdirectory(ihog)

# reasoning about scenes from a single image
add_subdirectory(depth_map)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()