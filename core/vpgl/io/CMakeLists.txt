# core/vpgl/io/Cmakelists.txt

SET(vpgl_io_sources
    vpgl_io_camera.h               vpgl_io_camera.txx      vpgl_io_camera.cxx
    vpgl_io_proj_camera.h             vpgl_io_proj_camera.txx
    vpgl_io_perspective_camera.h      vpgl_io_perspective_camera.txx
    vpgl_io_affine_camera.h           vpgl_io_affine_camera.txx
    vpgl_io_rational_camera.h         vpgl_io_rational_camera.txx
    vpgl_io_lvcs.h                    vpgl_io_lvcs.cxx
    vpgl_io_local_rational_camera.h   vpgl_io_local_rational_camera.txx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_io_sources)

ADD_LIBRARY(vpgl_io ${vpgl_io_sources})

TARGET_LINK_LIBRARIES(vpgl_io vsl vpgl vnl vgl vgl_algo vgl_io vnl_io vbl_io)

INSTALL_TARGETS(/lib vpgl_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/io ${vpgl_io_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
