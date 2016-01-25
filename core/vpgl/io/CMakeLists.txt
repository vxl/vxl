# core/vpgl/io/Cmakelists.txt

set(vpgl_io_sources
    vpgl_io_camera.h               vpgl_io_camera.txx      vpgl_io_camera.cxx
    vpgl_io_proj_camera.h             vpgl_io_proj_camera.txx
    vpgl_io_perspective_camera.h      vpgl_io_perspective_camera.txx
    vpgl_io_affine_camera.h           vpgl_io_affine_camera.txx
    vpgl_io_rational_camera.h         vpgl_io_rational_camera.txx
    vpgl_io_lvcs.h                    vpgl_io_lvcs.cxx
    vpgl_io_local_rational_camera.h   vpgl_io_local_rational_camera.txx
)

aux_source_directory(Templates vpgl_io_sources)

add_library(vpgl_io ${vpgl_io_sources})

target_link_libraries(vpgl_io vsl vpgl vnl vgl vgl_algo vgl_io vnl_io vbl_io)

install_targets(/lib vpgl_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/io ${vpgl_io_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
