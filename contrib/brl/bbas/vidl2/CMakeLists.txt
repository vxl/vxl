# contrib/brl/bbas/vidl2/CMakeLists.txt

PROJECT(vidl2)


INCLUDE( ${MODULE_PATH}/NewCMake/FindFFMPEG.cmake )
INCLUDE( ${MODULE_PATH}/NewCMake/FindDC1394.cmake )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

IF( FFMPEG_FOUND )
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_FFMPEG)
ENDIF( FFMPEG_FOUND )

# turn on to use DirectShow
#   this should be done with a try-compile not a user option
SET(DSHOW_FOUND OFF)
IF( DSHOW_FOUND )
  ADD_DEFINITIONS(-DHAS_DSHOW)
ENDIF( DSHOW_FOUND )


SET(vidl2_sources
  vidl2_pixel_format.h          vidl2_pixel_format.cxx
  vidl2_color.h                 vidl2_color.cxx
  vidl2_frame.h                 vidl2_frame.cxx               vidl2_frame_sptr.h
  vidl2_pixel_iterator.h        vidl2_pixel_iterator.cxx      vidl2_pixel_iterator.txx
  vidl2_convert.h               vidl2_convert.cxx

  vidl2_istream.h
  vidl2_image_list_istream.h    vidl2_image_list_istream.cxx
  vidl2_ostream.h
  vidl2_image_list_ostream.h    vidl2_image_list_ostream.cxx
  vidl2_iidc1394_params.h       vidl2_iidc1394_params.cxx
)

IF( FFMPEG_FOUND )
  SET(vidl2_sources ${vidl2_sources}
    vidl2_ffmpeg_init.h             vidl2_ffmpeg_init.cxx
    vidl2_ffmpeg_convert.h          vidl2_ffmpeg_convert.cxx
    vidl2_ffmpeg_istream.h          vidl2_ffmpeg_istream.cxx
    vidl2_ffmpeg_ostream.h          vidl2_ffmpeg_ostream.cxx
    vidl2_ffmpeg_ostream_params.h   vidl2_ffmpeg_ostream_params.cxx
  )
ENDIF( FFMPEG_FOUND )

IF( DC1394_FOUND )
  ADD_DEFINITIONS(-DHAS_DC1394)
  SET(vidl2_sources ${vidl2_sources}
    vidl2_dc1394_istream.h          vidl2_dc1394_istream.cxx
  )
ENDIF( DC1394_FOUND )

IF( DSHOW_FOUND )
  SET(vidl2_sources ${vidl2_sources}
    vidl2_exception.h
    vidl2_dshow.h            vidl2_dshow.cxx
    vidl2_dshow_file_istream.h       vidl2_dshow_file_istream.cxx
  )
ENDIF( DSHOW_FOUND )

AUX_SOURCE_DIRECTORY(Templates vidl2_sources)


ADD_LIBRARY(vidl2 ${vidl2_sources})
INSTALL_TARGETS(/lib vidl2)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/bbas/vidl2 ${vidl_sources})

TARGET_LINK_LIBRARIES( vidl2 vil vul )
IF( FFMPEG_FOUND )
  TARGET_LINK_LIBRARIES( vidl2 ${FFMPEG_LIBRARIES} )
ENDIF( FFMPEG_FOUND )

IF( DC1394_FOUND )
  TARGET_LINK_LIBRARIES( vidl2 ${DC1394_LIBRARIES} )
ENDIF( DC1394_FOUND )

IF( DSHOW_FOUND )
  TARGET_LINK_LIBRARIES( vidl2 strmiids quartz )
ENDIF( DSHOW_FOUND )


IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
