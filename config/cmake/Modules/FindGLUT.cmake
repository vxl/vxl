#
# try to find glut
#

IF(NOT HAS_GLUT)

  IF (WIN32)

    IF(CYGWIN)

      SET( HAS_GLUT "NO" )
  
      FIND_PATH( GLUT_INCLUDE_PATH GL/glut.h
        /usr/include
      )
  
      FIND_LIBRARY( GLUT_glut_LIBRARY glut32
        /usr/lib
        /usr/lib/w32api
      )

      IF(GLUT_INCLUDE_PATH)
        IF(GLUT_glut_LIBRARY)
          INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_PATH} )
          LINK_LIBRARIES( ${GLUT_glut_LIBRARY} )
          SET( HAS_GLUT "YES" )
          ADD_DEFINITIONS( -DHAS_GLUT )
        ENDIF(GLUT_glut_LIBRARY)
      ENDIF(GLUT_INCLUDE_PATH)

    ELSE(CYGWIN)

      SET( HAS_GLUT "NO" )
  
      FIND_PATH( GLUT_INCLUDE_PATH GL/glut.h 
        ${GLUT_ROOT_PATH}/include
      )
  
      FIND_LIBRARY( GLUT_glut_LIBRARY glut
        ${GLUT_ROOT_PATH}/Release
      )

      IF(GLUT_INCLUDE_PATH)
        IF(GLUT_glut_LIBRARY)
          INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_PATH} )
          LINK_LIBRARIES( ${GLUT_glut_LIBRARY} )
          SET( HAS_GLUT "YES" )
          ADD_DEFINITIONS( -DHAS_GLUT )
        ENDIF(GLUT_glut_LIBRARY)
      ENDIF(GLUT_INCLUDE_PATH)

    ENDIF(CYGWIN)

  ELSE (WIN32)
  
    SET( HAS_GLUT "NO" )
  
    FIND_PATH( GLUT_INCLUDE_PATH GL/glut.h
      /usr/include
      /usr/local/include
      /usr/openwin/share/include
      /usr/openwin/include
      /usr/X11R6/include
      /usr/include/X11
    )
  
    FIND_LIBRARY( GLUT_glut_LIBRARY glut
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )
  
    FIND_LIBRARY( GLUT_Xi_LIBRARY Xi
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )
  
    FIND_LIBRARY( GLUT_Xmu_LIBRARY Xmu
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )
  
  
    IF(GLUT_INCLUDE_PATH)
      IF(GLUT_glut_LIBRARY)
        INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_PATH} )
        LINK_LIBRARIES( ${GLUT_glut_LIBRARY} )
        SET( HAS_GLUT "YES" )
        ADD_DEFINITIONS( -DHAS_GLUT )
      ENDIF(GLUT_glut_LIBRARY)
  
      #
      # Is -lXi and -lXmu required on all platforms? If not, we need 
      # some way to figure out what platform we are on
      #
  
      IF(GLUT_Xi_LIBRARY)
        LINK_LIBRARIES( ${GLUT_Xi_LIBRARY} )
      ENDIF(GLUT_Xi_LIBRARY)
      IF(GLUT_Xmu_LIBRARY)
        LINK_LIBRARIES( ${GLUT_Xmu_LIBRARY} )
      ENDIF(GLUT_Xmu_LIBRARY)
    ENDIF(GLUT_INCLUDE_PATH)
  
  ENDIF (WIN32)

ENDIF(NOT HAS_GLUT)
