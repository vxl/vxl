# Find the FFmpeg library
#
# Sets
#   FFMPEG_FOUND.  If false, don't try to use ffmpeg
#   FFMPEG_FOUND_SEVERAL. If true, there are several directories with headers (not only ../ffmpeg/)
#   FFMPEG_INCLUDE_DIR
#   FFMPEG_LIBRARIES

SET( FFMPEG_FOUND "NO" )

FIND_PATH( FFMPEG_INCLUDE1_DIR ffmpeg/avcodec.h
  /usr/include
  /usr/local/include
)
FIND_PATH( FFMPEG_INCLUDE2_DIR libavcodec/avcodec.h
  /usr/include
  /usr/local/include
)
IF( FFMPEG_INCLUDE1_DIR)
  SET (FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE1_DIR} )
  SET( FFMPEG_FOUND_SEVERAL "NO" )
ENDIF ( FFMPEG_INCLUDE1_DIR)

IF( FFMPEG_INCLUDE2_DIR)
  SET (FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE2_DIR} )
  SET( FFMPEG_FOUND_SEVERAL "YES" )
ENDIF ( FFMPEG_INCLUDE2_DIR)

IF( FFMPEG_INCLUDE_DIR )

FIND_PROGRAM( FFMPEG_CONFIG ffmpeg-config
  /usr/bin
  /usr/local/bin
  ${HOME}/bin
)

IF( FFMPEG_CONFIG )
  EXEC_PROGRAM( ${FFMPEG_CONFIG} ARGS "--libs avformat" OUTPUT_VARIABLE FFMPEG_LIBS )
  SET( FFMPEG_FOUND "YES" )
  SET( FFMPEG_LIBRARIES "${FFMPEG_LIBS}" )

ELSE( FFMPEG_CONFIG )

  FIND_LIBRARY( FFMPEG_avcodec_LIBRARY avcodec
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )

  FIND_LIBRARY( FFMPEG_avformat_LIBRARY avformat
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )

  FIND_LIBRARY( FFMPEG_avutil_LIBRARY avutil
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )

  FIND_LIBRARY( FFMPEG_swscale_LIBRARY swscale
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )

  IF( FFMPEG_avcodec_LIBRARY )
  IF( FFMPEG_avformat_LIBRARY )

    SET( FFMPEG_FOUND "YES" )
    SET( FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY} ${FFMPEG_avcodec_LIBRARY} )
    IF( FFMPEG_avutil_LIBRARY )
       SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_avutil_LIBRARY} )
    ENDIF( FFMPEG_avutil_LIBRARY )
    IF( FFMPEG_swscale_LIBRARY )
       SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_swscale_LIBRARY} )
    ENDIF( FFMPEG_swscale_LIBRARY )

  ENDIF( FFMPEG_avformat_LIBRARY )
  ENDIF( FFMPEG_avcodec_LIBRARY )

ENDIF( FFMPEG_CONFIG )

ENDIF( FFMPEG_INCLUDE_DIR )
