#
# Find the native MPEG includes and library
#

IF(NOT HAS_NATIVE_MPEG)  


  FIND_PATH( NATIVE_MPEG_INCLUDE_PATH mpeg2dec/video_out.h
    /usr/local/include
    /usr/include
  )
  
  FIND_LIBRARY( NATIVE_MPEG_LIBRARY mpeg2lib
    /usr/lib
    /usr/local/lib
  )
  
  FIND_LIBRARY( NATIVE_MPEG_VO_LIBRARY libvo
    /usr/lib
    /usr/local/lib
  )
  
  IF(NATIVE_MPEG_INCLUDE_PATH)
   IF(NATIVE_MPEG_LIBRARY)
  
    SET( HAS_NATIVE_MPEG "YES" )

    SET( NATIVE_MPEG_LIBRARIES   ${NATIVE_MPEG_LIBRARY}
                                 ${NATIVE_MPEG_VO_LIBRARY} )
  
   ENDIF(NATIVE_MPEG_LIBRARY)
  ENDIF(NATIVE_MPEG_INCLUDE_PATH)
ENDIF(NOT HAS_NATIVE_MPEG)
