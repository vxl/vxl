SET( b3p_netlib_sources

    b3p_netlib.h
    
    # composition simpson and trapozoid integration
    simpson.cxx
    adaquad.cxx
    trapezod.cxx
)

INCLUDE_DIRECTORIES(${vthreep_SOURCE_DIR}/netlib)
INCLUDE_DIRECTORIES(${bthreep_SOURCE_DIR}/netlib)
ADD_LIBRARY( b3p_netlib ${b3p_netlib_sources} )

#IF( UNIX )
#  TARGET_LINK_LIBRARIES( b3p_netlib_sources m )
#ENDIF (UNIX)

IF( BUILD_TESTING)
  SUBDIRS( tests )
ENDIF( BUILD_TESTING)
