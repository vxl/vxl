#
# Find the native MPEG includes and library
#

IF(NOT HAS_NATIVE_MPEG)

  FIND_PATH( NATIVE_MPEG_INCLUDE_PATH mpeg2dec/include/video_out.h
    /usr/local
    /usr
    /usr/local/livid
  )

  FIND_LIBRARY( NATIVE_MPEG2_LIBRARY mpeg2
    /usr/local/lib
    /usr/lib
    /usr/local/livid/mpeg2dec/libmpeg2/.libs
  )

  FIND_LIBRARY( NATIVE_MPEG_VO_LIBRARY vo
    /usr/local/lib
    /usr/lib
    /usr/local/livid/mpeg2dec/libvo/.libs
  )

  IF(NATIVE_MPEG_INCLUDE_PATH)
   IF(NATIVE_MPEG2_LIBRARY)
      SET( HAS_NATIVE_MPEG "YES" )
      SET( NATIVE_MPEG_LIBRARIES ${NATIVE_MPEG2_LIBRARY}
                                 ${NATIVE_MPEG_VO_LIBRARY} )
    IF(CMAKE_SYSTEM MATCHES "Linux*")
      SET( NATIVE_MPEG_LIBRARIES ${NATIVE_MPEG_LIBRARIES} Xv )
    ENDIF(CMAKE_SYSTEM MATCHES "Linux*")
   ENDIF(NATIVE_MPEG2_LIBRARY)
  ENDIF(NATIVE_MPEG_INCLUDE_PATH)
ENDIF(NOT HAS_NATIVE_MPEG)
