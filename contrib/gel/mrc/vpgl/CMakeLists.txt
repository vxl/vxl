# gel/mrc/vpgl/CMakeLists.txt
# Photogrametry Library.

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc)

SET( vpgl_sources
  
  vgl_rotation_3d.h

  vpgl_camera.h                  
  vpgl_proj_camera.h            vpgl_proj_camera.txx
  vpgl_calibration_matrix.h     vpgl_calibration_matrix.txx
  vpgl_perspective_camera.h     vpgl_perspective_camera.txx
  vpgl_affine_camera.h          vpgl_affine_camera.txx
  vpgl_fundamental_matrix.h     vpgl_fundamental_matrix.txx  
  vpgl_reg_fundamental_matrix.h vpgl_reg_fundamental_matrix.txx 
  vpgl_affine_fundamental_matrix.h  vpgl_affine_fundamental_matrix.txx  
  vpgl_lens_distortion.h        vpgl_lens_distortion.txx
  vpgl_radial_distortion.h      vpgl_radial_distortion.txx
  vpgl_poly_radial_distortion.h vpgl_poly_radial_distortion.txx   
  vpgl_rational_camera.h        vpgl_rational_camera.txx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_sources)

ADD_LIBRARY(vpgl ${vpgl_sources})
TARGET_LINK_LIBRARIES( vpgl vnl vnl_algo vnl_io vgl vgl_algo vgl_io vgl_xio vsl)


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(algo)
