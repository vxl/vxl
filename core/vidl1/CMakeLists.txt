# vxl/vidl1/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )
INCLUDE( ${MODULE_PATH}/NewCMake/FindFFMPEG.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )

IF( FFMPEG_FOUND )
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_FFMPEG)
# some versions of FFMPEG require this definition before including
# the headers for C++ compatibility
  ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS )
ENDIF( FFMPEG_FOUND )

if ( FFMPEG_FOUND_SEVERAL )
  ADD_DEFINITIONS(-DHAS_FFMPEG_SEVERAL)
ENDIF ( FFMPEG_FOUND_SEVERAL )

SET(vidl1_sources
  dll.h

  vidl1_vob_frame_index.cxx    vidl1_vob_frame_index.h
  vidl1_file_sequence.cxx      vidl1_file_sequence.h
  vidl1_codec.cxx vidl1_codec.h vidl1_codec_sptr.h
  vidl1_clip.cxx  vidl1_clip.h  vidl1_clip_sptr.h
  vidl1_movie.cxx vidl1_movie.h vidl1_movie_sptr.h
  vidl1_frame.cxx vidl1_frame.h vidl1_frame_sptr.h
  vidl1_frame_resource.cxx     vidl1_frame_resource.h
  vidl1_image_list_codec.cxx vidl1_image_list_codec.h vidl1_image_list_codec_sptr.h
  vidl1_io.cxx    vidl1_io.h
)

IF( MPEG2_FOUND )
  SET(vidl1_sources ${vidl1_sources}
    vidl1_mpegcodec.cxx vidl1_mpegcodec.h vidl1_mpegcodec_sptr.h
    vidl1_mpegcodec_helper.cxx vidl1_mpegcodec_helper.h
    vidl1_yuv_2_rgb.h
  )
ENDIF( MPEG2_FOUND )

IF( FFMPEG_FOUND )
  SET(vidl1_sources ${vidl1_sources}
    vidl1_ffmpeg_codec.cxx vidl1_ffmpeg_codec.h
  )
ENDIF( FFMPEG_FOUND )

AUX_SOURCE_DIRECTORY(Templates vidl1_sources)


IF (NOT AVIFILE_FOUND)
  INCLUDE(${CMAKE_ROOT}/Modules/FindAVIFile.cmake)
  IF(AVIFILE_FOUND)
    INCLUDE_DIRECTORIES( ${AVIFILE_INCLUDE_DIR} )
    ADD_DEFINITIONS (${AVIFILE_DEFINITIONS} )
  ENDIF(AVIFILE_FOUND)
ENDIF (NOT AVIFILE_FOUND)

SET(AVI_FOUND "NO")
IF (AVIFILE_FOUND)
  SET(vidl1_sources ${vidl1_sources}
      vidl1_avicodec.h
      vidl1_avifile_avicodec.h vidl1_avifile_avicodec.cxx
     )
  SET(AVI_LIBRARIES ${AVIFILE_LIBRARIES})
  SET(AVI_FOUND "YES")
  ADD_DEFINITIONS( -DHAS_AVI )
ELSE (AVIFILE_FOUND)
  IF (WIN32)
    IF( NOT CYGWIN )
      SET(vidl1_sources ${vidl1_sources}
          vidl1_avicodec.h
          vidl1_win_avicodec.h vidl1_win_avicodec.cxx
         )

      # Borland compilers may fail to link vfw32.lib if they are not
      # installed correctly.  Make sure the ilink32.cfg file
      # lists the directory containing the library in the -L option.
      # For example: -L"c:\Borland\BCC55\Lib";"c:\Borland\BCC55\Lib\PSDK"
      SET(AVI_LIBRARIES vfw32 gdi32)
      SET(AVI_FOUND "YES")
      ADD_DEFINITIONS( -DHAS_AVI )
    ENDIF( NOT CYGWIN )
  ENDIF (WIN32)
ENDIF (AVIFILE_FOUND)


ADD_LIBRARY(vidl1 ${vidl1_sources})
INSTALL_TARGETS(/lib vidl1)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vidl1 ${vidl1_sources})

TARGET_LINK_LIBRARIES( vidl1 vbl vul vil vcl )
IF( MPEG2_FOUND )
  TARGET_LINK_LIBRARIES( vidl1 ${MPEG2_LIBRARIES} )
ENDIF( MPEG2_FOUND )
IF( FFMPEG_FOUND )
  TARGET_LINK_LIBRARIES( vidl1 ${FFMPEG_LIBRARIES} )
ENDIF( FFMPEG_FOUND )
IF( AVI_FOUND )
  TARGET_LINK_LIBRARIES( vidl1 ${AVI_LIBRARIES} )
ENDIF( AVI_FOUND )

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
