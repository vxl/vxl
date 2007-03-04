# gel/mrc/vpgl/file_formats/CMakeLists.txt
# various file_formats for camera parameter I/O
# file format could include image files with embedded camera models
# so make this libary separate to avoid cross linking unless desired
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)

SET( vpgl_file_formats_sources
  vpgl_nitf_rational_camera.h  vpgl_nitf_rational_camera.cxx
)

ADD_LIBRARY(vpgl_file_formats ${vpgl_file_formats_sources})

TARGET_LINK_LIBRARIES(vpgl_file_formats vpgl vil)
