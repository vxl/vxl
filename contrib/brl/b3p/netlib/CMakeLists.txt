SET( b3p_netlib_sources

    b3p_netlib.h
    
    # composition simpson and trapozoid integration
    simpson.c
    adaquad.c
    trapezod.c
)

ADD_LIBRARY( b3p_netlib ${b3p_netlib_sources} )

IF( BUILD_TESTING)
  SUBDIRS( tests )
ENDIF( BUILD_TESTING)
