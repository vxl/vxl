# vxl/vpgl/CMakeLists.txt
# Photogrammetry Library.

doxygen_add_library(core/vpgl
  DEPENDS core/vcsl core/vgl core/vnl core/vbl
  PACKAGE core-L2
  DESCRIPTION "Photogrammetry Library"
  )


set( vpgl_sources
  vpgl_camera.h                    vpgl_camera_double_sptr.h
                                   vpgl_camera_float_sptr.h
  vpgl_proj_camera.h               vpgl_proj_camera.hxx
  vpgl_calibration_matrix.h        vpgl_calibration_matrix.hxx
  vpgl_perspective_camera.h        vpgl_perspective_camera.hxx
  vpgl_affine_camera.h             vpgl_affine_camera.hxx
  vpgl_fundamental_matrix.h        vpgl_fundamental_matrix.hxx
  vpgl_essential_matrix.h          vpgl_essential_matrix.hxx
  vpgl_affine_fundamental_matrix.h vpgl_affine_fundamental_matrix.hxx
  vpgl_lens_distortion.h           vpgl_lens_distortion.hxx
  vpgl_radial_distortion.h         vpgl_radial_distortion.hxx
  vpgl_poly_radial_distortion.h    vpgl_poly_radial_distortion.hxx
  vpgl_rational_camera.h           vpgl_rational_camera.hxx
  vpgl_local_rational_camera.h     vpgl_local_rational_camera.hxx
  vpgl_generic_camera.h            vpgl_generic_camera.hxx
  vpgl_dll.h
  vpgl_lvcs.h                      vpgl_lvcs.cxx        vpgl_lvcs_sptr.h
  vpgl_utm.h                       vpgl_utm.cxx
  vpgl_earth_constants.h
  vpgl_datum_conversion.h          vpgl_datum_conversion.cxx
)

aux_source_directory(Templates vpgl_sources)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpgl LIBRARY_SOURCES ${vpgl_sources})
target_link_libraries(${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl)
set(CURR_LIB_NAME vpgl)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     #USE_HIDDEN_VISIBILITY
)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(algo)
add_subdirectory(file_formats)
add_subdirectory(io)
add_subdirectory(xio)
