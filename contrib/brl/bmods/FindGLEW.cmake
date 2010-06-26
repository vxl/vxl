#
# try to find glew (OpenGL Extensions Wrangler Libarary) libarary and include files
#
# GLEW_INCLUDE_DIR, where to find GL/glew.h, etc.
# GLEW_LIBRARIES, the libraries to link against to use GLEW.
# GLEW_FOUND, If false, do not try to use GLEW.


IF (WIN32)

  IF(CYGWIN)

    FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
      ${OPENGL_INCLUDE_DIR}
    )

    FIND_LIBRARY( GLEW_LIBRARY glew32
      ${OPENGL_LIBRARY_DIR}
      /usr/lib64
      /usr/lib
      /usr/lib/w32api
      /usr/local/lib
      /usr/X11R6/lib
    )


  ELSE(CYGWIN)

    FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
      ${OPENGL_INCLUDE_DIR}
    )

    FIND_LIBRARY( GLEW_LIBRARY glew64
      ${OPENGL_LIBRARY_DIR}
    )

  ENDIF(CYGWIN)

ELSE (WIN32)

    FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
      /usr/include
      /usr/include/GL
      /usr/local/include
      /usr/openwin/share/include
      /usr/openwin/include
      /usr/X11R6/include
      /usr/include/X11
      /opt/graphics/OpenGL/include
      /opt/graphics/OpenGL/contrib/libglew
      ${OPENGL_INCLUDE_DIR}
    )

    FIND_LIBRARY( GLEW_LIBRARY glew
      /usr/lib64
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
      ${OPENGL_LIBRARY_DIR}
    )

ENDIF (WIN32)

SET( GLEW_FOUND "NO" )
IF(GLEW_INCLUDE_DIR)
  IF(GLEW_LIBRARY)
    SET( GLEW_FOUND "YES" )
  ENDIF(GLEW_LIBRARY)
ENDIF(GLEW_INCLUDE_DIR)

MARK_AS_ADVANCED(
  GLEW_INCLUDE_DIR
  GLEW_LIBRARY
)
