# ./vxl/vidl/CMakeLists.txt

ADD_LIBRARY(vidl vidl_sources)

SOURCE_FILES(vidl_sources
  vidl_clip.cxx
  vidl_movie.cxx
  vidl_frame.cxx
  vidl_frame_as_image.cxx
  vidl_image_list_codec.cxx
  vidl_io.cxx
  vidl_test.cxx
)

AUX_SOURCE_DIRECTORY(Templates vidl_sources)

IF(WIN32)
  IF (NOT CYGWIN)
    SOURCE_FILES(vidl_sources vidl_avicodec.cxx)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
