# vxl/vidl/CMakeLists.txt

ADD_LIBRARY(vidl vidl_sources)

INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake )

SOURCE_FILES(vidl_sources
  vidl_test.cxx

  vidl_codec.h vidl_codec_sptr.h
  vidl_clip.cxx vidl_clip.h vidl_clip_sptr.h
  vidl_movie.cxx vidl_movie.h vidl_movie_sptr.h
  vidl_frame.cxx vidl_frame.h vidl_frame_sptr.h
  vidl_frame_as_image.cxx vidl_frame_as_image.h vidl_frame_as_image_sptr.h
  vidl_image_list_codec.cxx vidl_image_list_codec.h vidl_image_list_codec_sptr.h
  vidl_io.cxx vidl_io.h
)

IF( HAS_MPEG )
  SOURCE_FILES(vidl_sources
    vidl_mpegcodec.cxx vidl_mpegcodec.h vidl_mpegcodec_sptr.h
    vidl_mpegcodec_helper.cxx vidl_mpegcodec_helper.h	
    vidl_yuv_2_rgb.cxx vidl_yuv_2_rgb.h
  )
ENDIF( HAS_MPEG )

AUX_SOURCE_DIRECTORY(Templates vidl_sources)

IF(WIN32)
  IF (NOT CYGWIN)
    SOURCE_FILES(vidl_sources
      vidl_avicodec.cxx vidl_avicodec.h vidl_avicodec_sptr.h
    )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
