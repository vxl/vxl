# gel/mrc/vpgl/io/Cmakelists.txt


SET(vpgl_io_sources
    vpgl_io_camera.h vpgl_io_camera.txx
    vpgl_io_rational_camera.h 
    vpgl_io_proj_camera.h 
)

AUX_SOURCE_DIRECTORY(Templates vpgl_io_sources)

ADD_LIBRARY(vpgl_io ${vpgl_io_sources})
TARGET_LINK_LIBRARIES(vpgl_io vsl vpgl vnl vnl_io)


