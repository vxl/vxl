#
# try to find glut
#

# don't even bother under WIN32
IF (UNIX)

  FIND_PATH(GLUT_INCLUDE_PATH GL/glut.h
  /usr/include
  /usr/local/include
  /usr/openwin/share/include
  /usr/X11R6/include
  )

  FIND_LIBRARY(GLUT_LIB_PATH glut
  /usr/lib
  /usr/local/lib
  /usr/openwin/lib
  /usr/X11R6/lib
  )

  IF(GLUT_INCLUDE_PATH)
    SET(GLUT_LIBRARY "-lglut" CACHE)
    ADD_DEFINITIONS( -DHAS_GLUT )
  ENDIF(GLUT_INCLUDE_PATH)

ENDIF (UNIX)

