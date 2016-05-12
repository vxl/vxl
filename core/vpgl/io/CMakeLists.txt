# core/vpgl/io/Cmakelists.txt

set(vpgl_io_sources
    vpgl_io_camera.h               vpgl_io_camera.hxx      vpgl_io_camera.cxx
    vpgl_io_proj_camera.h             vpgl_io_proj_camera.hxx
    vpgl_io_perspective_camera.h      vpgl_io_perspective_camera.hxx
    vpgl_io_affine_camera.h           vpgl_io_affine_camera.hxx
    vpgl_io_rational_camera.h         vpgl_io_rational_camera.hxx
    vpgl_io_lvcs.h                    vpgl_io_lvcs.cxx
    vpgl_io_local_rational_camera.h   vpgl_io_local_rational_camera.hxx
)

aux_source_directory(Templates vpgl_io_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpgl_io LIBRARY_SOURCES ${vpgl_io_sources})

target_link_libraries(${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
