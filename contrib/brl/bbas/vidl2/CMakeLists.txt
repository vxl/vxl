# contrib/brl/bbas/vidl2/CMakeLists.txt

PROJECT(vidl2)


INCLUDE( ${MODULE_PATH}/NewCMake/FindFFMPEG.cmake )
INCLUDE( ${MODULE_PATH}/NewCMake/FindDC1394.cmake )
INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

IF( FFMPEG_FOUND )
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_FFMPEG)
ENDIF( FFMPEG_FOUND )

IF(DIRECTSHOW_FOUND)
  INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_DSHOW)

  # For Euresys Multicam Driver's DirectShow source filter.
  # - should I put this in a Module of it's own?
  #INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow_ESF.cmake )
  FIND_PATH(DIRECTSHOW_ESF_INCLUDE_DIR ESFilter.h
    "C:/Program Files/Euresys/MultiCam/Include"
    DOC "What is the path where the file ESFilter.h can be found"
  )
  IF(DIRECTSHOW_ESF_INCLUDE_DIR)
    SET(DIRECTSHOW_ESF_FOUND "YES")
  ENDIF(DIRECTSHOW_ESF_INCLUDE_DIR)

  IF(DIRECTSHOW_ESF_FOUND)
    INCLUDE_DIRECTORIES(${DIRECTSHOW_ESF_INCLUDE_DIR})
    ADD_DEFINITIONS(-DHAS_DSHOW_ESF)
  ENDIF(DIRECTSHOW_ESF_FOUND)
ENDIF(DIRECTSHOW_FOUND)


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

IF(DIRECTSHOW_FOUND)
  SET(vidl2_sources ${vidl2_sources}
    vidl2_exception.h
    vidl2_dshow.h                      vidl2_dshow.cxx
    vidl2_dshow_file_istream.h         vidl2_dshow_file_istream.cxx
    vidl2_dshow_live_istream.h         vidl2_dshow_live_istream.cxx
    vidl2_dshow_istream_params.h       vidl2_dshow_istream_params.cxx
  )
  IF(DIRECTSHOW_ESF_FOUND)
    SET(vidl2_sources ${vidl2_sources}
      vidl2_dshow_istream_params_esf.h   vidl2_dshow_istream_params_esf.cxx
    )
  ENDIF(DIRECTSHOW_ESF_FOUND)
ENDIF(DIRECTSHOW_FOUND)

IF(UNIX)
   SET(vidl2_sources ${vidl2_sources}
       vidl2_v4l_params.h vidl2_v4l_params.cxx
       vidl2_v4l_istream.h vidl2_v4l_istream.cxx
   )
ENDIF(UNIX)


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

IF(DIRECTSHOW_FOUND)
  TARGET_LINK_LIBRARIES( vidl2 ${DIRECTSHOW_LIBRARIES} )
ENDIF(DIRECTSHOW_FOUND)


IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
