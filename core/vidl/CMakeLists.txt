#
#  vxl/vidl/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

ADD_LIBRARY(vidl vidl_sources)

SOURCE_FILES(vidl_sources
vidl_clip
vidl_movie
vidl_frame
vidl_frame_as_image
vidl_image_list_codec
vidl_io
vidl_test
)

IF(WIN32)
  SOURCE_FILES(vidl_sources
    vidl_avicodec
  )
ENDIF(WIN32)


AUX_SOURCE_DIRECTORY(Templates vidl_sources)
