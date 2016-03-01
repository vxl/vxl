# vxl/vpgl/CMakeLists.txt
# Photogrammetry Library.

doxygen_add_library(core/vpgl
  DEPENDS core/vcsl core/vgl core/vnl core/vbl
  PACKAGE core-L2
  DESCRIPTION "Photogrammetry Library"
  )

include_directories(${VXLCORE_INCLUDE_DIR})

set( vpgl_sources
  vpgl_export.h
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

vxl_add_library(LIBNAME ${VXL_LIB_PREFIX}vpgl LIBSRCS ${vpgl_sources})
target_link_libraries(${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(algo)
add_subdirectory(file_formats)
add_subdirectory(io)
add_subdirectory(xio)
