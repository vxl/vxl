# vxl/vpgl/CMakeLists.txt
# Photogrammetry Library.

doxygen_add_library(core/vpgl
  DEPENDS core/vcsl core/vgl core/vnl core/vbl
  PACKAGE core-L2
  DESCRIPTION "Photogrammetry Library"
  )

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})

SET( vpgl_sources
  vpgl_camera.h                    vpgl_camera_double_sptr.h
                                   vpgl_camera_float_sptr.h
  vpgl_proj_camera.h               vpgl_proj_camera.txx
  vpgl_calibration_matrix.h        vpgl_calibration_matrix.txx
  vpgl_perspective_camera.h        vpgl_perspective_camera.txx
  vpgl_affine_camera.h             vpgl_affine_camera.txx
  vpgl_fundamental_matrix.h        vpgl_fundamental_matrix.txx
  vpgl_essential_matrix.h          vpgl_essential_matrix.txx
  vpgl_affine_fundamental_matrix.h vpgl_affine_fundamental_matrix.txx
  vpgl_rational_camera.h           vpgl_rational_camera.txx
  vpgl_local_rational_camera.h     vpgl_local_rational_camera.txx
  vpgl_generic_camera.h            vpgl_generic_camera.txx
  vpgl_dll.h
  vpgl_lvcs.h                      vpgl_lvcs.cxx	vpgl_lvcs_sptr.h
  vpgl_utm.h                       vpgl_utm.cxx
  vpgl_earth_constants.h
  vpgl_datum_conversion.h          vpgl_datum_conversion.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_sources)

ADD_LIBRARY(vpgl ${vpgl_sources})
TARGET_LINK_LIBRARIES(vpgl vnl_algo vnl vgl_algo vgl vul vsl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(algo)
SUBDIRS(file_formats)
SUBDIRS(io)
SUBDIRS(xio)
INSTALL_TARGETS(/lib vpgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/ ${vpgl_sources})
