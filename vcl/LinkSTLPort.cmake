# STLPort link settings


IF(USE_STLPORT)
  LINK_DIRECTORIES ( ${STLPORT}/lib )

  IF(WIN32)
    IF(CYGWIN)
      IF(CMAKE_BUILD_TYPE MATCHES "Debug")
          TARGET_LINK_LIBRARIES( vcl   ${STLPORT}/lib/libstlport_gcc_stldebug.a )
      ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
          TARGET_LINK_LIBRARIES( vcl  ${STLPORT}/lib/libstlport_gcc.a )
      ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

#      LINK_LIBRARIES( debug ${STLPORT}/lib/libstlport_cygwin_stldebug.a )
#      LINK_LIBRARIES( optimized ${STLPORT}/lib/libstlport_cygwin.a )
    ENDIF(CYGWIN)
  ELSE(WIN32)
    IF(UNIX)
      TARGET_LINK_LIBRARIES( vcl   pthread )

      IF(CMAKE_BUILD_TYPE MATCHES "Debug")
          TARGET_LINK_LIBRARIES( vcl   ${STLPORT}/lib/libstlport_gcc_stldebug.a )
      ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
          TARGET_LINK_LIBRARIES( vcl  ${STLPORT}/lib/libstlport_gcc.a )
      ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

    ENDIF(UNIX)
  ENDIF(WIN32)

ENDIF(USE_STLPORT)
