INCLUDE( ${MODULE_PATH}/FindNetlib.cmake )

IF(NETLIB_FOUND)
  INCLUDE_DIRECTORIES( ${NETLIB_INCLUDE_DIR} )
ENDIF(NETLIB_FOUND)

SET(b3p_netlib_sources

    
    # composition simpson and trapozoid integration
   )

ADD_LIBRARY( b3p_netlib ${b3p_netlib_sources} )

IF( BUILD_TESTING)
  SUBDIRS( tests )
ENDIF( BUILD_TESTING)
