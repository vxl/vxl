# vxl/vidl/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )

SET(vidl_sources
  vidl_vob_frame_index.cxx    vidl_vob_frame_index.h
  vidl_file_sequence.cxx      vidl_file_sequence.h
  vidl_codec.h                vidl_codec_sptr.h
  vidl_clip.cxx  vidl_clip.h  vidl_clip_sptr.h
  vidl_movie.cxx vidl_movie.h vidl_movie_sptr.h
  vidl_frame.cxx vidl_frame.h vidl_frame_sptr.h
  vidl_image_list_codec.cxx vidl_image_list_codec.h vidl_image_list_codec_sptr.h
  vidl_io.cxx    vidl_io.h
)

IF( MPEG2_FOUND )
  SET(vidl_sources ${vidl_sources}
    vidl_mpegcodec.cxx vidl_mpegcodec.h vidl_mpegcodec_sptr.h
    vidl_mpegcodec_helper.cxx vidl_mpegcodec_helper.h
    vidl_yuv_2_rgb.cxx vidl_yuv_2_rgb.h
  )
ENDIF( MPEG2_FOUND )

AUX_SOURCE_DIRECTORY(Templates vidl_sources)


IF (NOT AVIFILE_FOUND)
  INCLUDE(${CMAKE_ROOT}/Modules/FindAVIFile.cmake)
  IF(AVIFILE_FOUND)
    INCLUDE_DIRECTORIES( ${AVIFILE_INCLUDE_DIR} )
    ADD_DEFINITIONS (${AVIFILE_DEFINITIONS} )
  ENDIF(AVIFILE_FOUND)
ENDIF (NOT AVIFILE_FOUND)

IF (AVIFILE_FOUND)
  SET(vidl_sources ${vidl_sources}
      vidl_avicodec.h
      vidl_avifile_avicodec.h vidl_avifile_avicodec.cxx
     )
  SET(AVI_LIBRARIES ${AVIFILE_LIBRARIES})
  SET(AVI_FOUND "YES")
  ADD_DEFINITIONS( -DHAS_AVI )
ELSE (AVIFILE_FOUND)
  IF (WIN32)
    IF( NOT CYGWIN )
      SET(vidl_sources ${vidl_sources}
          vidl_win_avicodec.h vidl_win_avicodec.cxx
         )

      # Borland compilers may fail to link vfw32.lib if they are not
      # installed correctly.  Make sure the ilink32.cfg file
      # lists the directory containing the library in the -L option.
      # For example: -L"c:\Borland\BCC55\Lib";"c:\Borland\BCC55\Lib\PSDK"
      SET(AVI_LIBRARIES "vfw32 gdi32")
      SET(AVI_FOUND "YES")
      ADD_DEFINITIONS( -DHAS_AVI )
    ENDIF( NOT CYGWIN )
  ENDIF (WIN32)
  SET(AVI_LIBRARIES "")
ENDIF (AVIFILE_FOUND)


ADD_LIBRARY(vidl ${vidl_sources})
TARGET_LINK_LIBRARIES( vidl vbl vul vil vcl )
IF( MPEG2_FOUND )
  TARGET_LINK_LIBRARIES( vidl ${MPEG2_LIBRARIES} )
ENDIF( MPEG2_FOUND )
IF( AVI_FOUND )
  TARGET_LINK_LIBRARIES( vidl ${AVI_LIBRARIES} )
ENDIF( AVI_FOUND )

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
