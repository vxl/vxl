# vxl/vidl_vil1/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )

SET(vidl_vil1_sources
  vidl_vil1_vob_frame_index.cxx    vidl_vil1_vob_frame_index.h
  vidl_vil1_file_sequence.cxx      vidl_vil1_file_sequence.h
  vidl_vil1_codec.h                vidl_vil1_codec_sptr.h
  vidl_vil1_clip.cxx  vidl_vil1_clip.h  vidl_vil1_clip_sptr.h
  vidl_vil1_movie.cxx vidl_vil1_movie.h vidl_vil1_movie_sptr.h
  vidl_vil1_frame.cxx vidl_vil1_frame.h vidl_vil1_frame_sptr.h
  vidl_vil1_frame_as_image.cxx vidl_vil1_frame_as_image.h vidl_vil1_frame_as_image_sptr.h
  vidl_vil1_image_list_codec.cxx vidl_vil1_image_list_codec.h vidl_vil1_image_list_codec_sptr.h
  vidl_vil1_io.cxx    vidl_vil1_io.h
)

IF( MPEG2_FOUND )
  SET(vidl_vil1_sources ${vidl_vil1_sources}
    vidl_vil1_mpegcodec.cxx vidl_vil1_mpegcodec.h vidl_vil1_mpegcodec_sptr.h
    vidl_vil1_mpegcodec_helper.cxx vidl_vil1_mpegcodec_helper.h
    vidl_vil1_yuv_2_rgb.h
  )
ENDIF( MPEG2_FOUND )

AUX_SOURCE_DIRECTORY(Templates vidl_vil1_sources)

IF(WIN32)
  IF (NOT CYGWIN)
    SET(vidl_vil1_sources ${vidl_vil1_sources}
      vidl_vil1_avicodec.cxx vidl_vil1_avicodec.h vidl_vil1_avicodec_sptr.h
    )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ADD_LIBRARY(vidl_vil1 ${vidl_vil1_sources})
TARGET_LINK_LIBRARIES( vidl_vil1 vbl vul vil1 vcl )
IF( MPEG2_FOUND )
  TARGET_LINK_LIBRARIES( vidl_vil1 ${MPEG2_LIBRARIES} )
ENDIF( MPEG2_FOUND )

IF (WIN32)
  IF (NOT CYGWIN)
    # Borland compilers may fail to link vfw32.lib if they are not
    # installed correctly.  Make sure the ilink32.cfg file
    # lists the directory containing the library in the -L option.
    # For example: -L"c:\Borland\BCC55\Lib";"c:\Borland\BCC55\Lib\PSDK"
    TARGET_LINK_LIBRARIES( vidl_vil1 vfw32 gdi32 )
  ENDIF (NOT CYGWIN)
ENDIF (WIN32)

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
