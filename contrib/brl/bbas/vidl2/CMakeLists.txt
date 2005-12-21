# contrib/brl/bbas/vidl2/CMakeLists.txt


INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

IF( FFMPEG_FOUND )
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_FFMPEG)
ENDIF( FFMPEG_FOUND )


SET(vidl2_sources
  vidl2_istream.h
  vidl2_image_list_istream.h    vidl2_image_list_istream.cxx
  vidl2_ostream.h
  vidl2_image_list_ostream.h    vidl2_image_list_ostream.cxx
)


IF( FFMPEG_FOUND )
  SET(vidl_sources ${vidl_sources}
#    vidl2_ffmpeg_istream.cxx   vidl2_ffmpeg_istream.h
#    vidl2_ffmpeg_ostream.cxx   vidl2_ffmpeg_ostream.h
  )
ENDIF( FFMPEG_FOUND )

#AUX_SOURCE_DIRECTORY(Templates vidl_sources)



ADD_LIBRARY(vidl2 ${vidl2_sources})
INSTALL_TARGETS(/lib vidl2)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/bbas/vidl2 ${vidl_sources})

TARGET_LINK_LIBRARIES( vidl2 vil vul )
IF( FFMPEG_FOUND )
  TARGET_LINK_LIBRARIES( vidl ${FFMPEG_LIBRARIES} )
ENDIF( FFMPEG_FOUND )


IF( BUILD_TESTING )
#  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
