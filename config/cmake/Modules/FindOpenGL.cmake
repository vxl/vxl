#
# try to find OpenGL on UNIX systems once done this will define
#
# HAS_OPENGL          = system has OpenGL and it should be used
#
# OPENGL_INCLUDE_PATH = where the GL include directory can be found
# OPENGL_LIB_PATH     = where the GL library can be found
# OPENGL_LIBRARY      = the name of the OpenGL library to link with,
#                       or "not available" if the search was unsuccessful
#
# If the variable OPENGL_LIBRARY is set, we assume that OpenGL is
# present. We cannot use OPENGL_INCLUDE_PATH, because sometimes the
# library is present but there isn't a special include path. (E.g. on
# Windows)
#

IF(NOT HAS_OPENGL)
  IF (WIN32)

    IF (CYGWIN)
  
      SET( HAS_OPENGL "NO" )
  
      FIND_PATH(OPENGL_INCLUDE_PATH GL/gl.h 
        /usr/include 
        /usr/X11R6/include 
      )
  
      FIND_LIBRARY(OPENGL_gl_LIBRARY opengl32
        /usr/lib 
        /usr/lib/w32api
      )
  
      FIND_LIBRARY(OPENGL_glu_LIBRARY glu32
        /usr/lib 
        /usr/lib/w32api
      )
  
      IF(OPENGL_INCLUDE_PATH)
      IF(OPENGL_gl_LIBRARY)
      IF(OPENGL_glu_LIBRARY)
  
        SET( HAS_OPENGL "YES" )
        ADD_DEFINITIONS( -DHAS_OPENGL )

        INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
        LINK_LIBRARIES( ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} )
        SET( OPENGL_LIBRARIES ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} )

      ENDIF(OPENGL_glu_LIBRARY)
      ENDIF(OPENGL_gl_LIBRARY)
      ENDIF(OPENGL_INCLUDE_PATH)

    ELSE (CYGWIN)

      SET (HAS_OPENGL "YES")
      ADD_DEFINITIONS( -DHAS_OPENGL )
      LINK_LIBRARIES(opengl32 glu32)
      SET( OPENGL_LIBRARIES opengl32 glu32 )
      # needs to be set to something, but no special include path needed
      SET( OPENGL_INCLUDE_PATH "" )

    ENDIF (CYGWIN)

  ELSE (WIN32)
  
    SET( HAS_OPENGL "NO" )
  
    # The first line below is to make sure that the proper headers
    # are used on a Linux machine with the NVidia drivers installed.
    # They replace Mesa with NVidia's own library but normally do not
    # install headers and that causes the linking of parts of vgui to
    # fail since the compiler finds the Mesa headers but NVidia's library.
    # Make sure the NVIDIA directory comes BEFORE the others.
    #  - Atanas Georgiev <atanas@cs.columbia.edu>

    FIND_PATH(OPENGL_INCLUDE_PATH GL/gl.h 
      /usr/share/doc/NVIDIA_GLX-1.0/include
      /usr/include 
      /usr/local/include 
      /usr/openwin/share/include 
      /opt/graphics/OpenGL/include 
      /usr/X11R6/include 
    )
  
    FIND_LIBRARY(OPENGL_gl_LIBRARY
      NAMES MesaGL GL
      PATHS /usr/lib 
            /usr/local/lib 
            /opt/graphics/OpenGL/lib 
            /usr/openwin/lib 
            /usr/X11R6/lib
    )
  
    FIND_LIBRARY(OPENGL_glu_LIBRARY
      NAMES MesaGLU GLU
      PATHS /usr/lib 
            /usr/local/lib 
            /opt/graphics/OpenGL/lib 
            /usr/openwin/lib 
            /usr/X11R6/lib
    )
  
    IF(OPENGL_INCLUDE_PATH)
    IF(OPENGL_gl_LIBRARY)
    IF(OPENGL_glu_LIBRARY)
  
      SET( HAS_OPENGL "YES" )
      ADD_DEFINITIONS( -DHAS_OPENGL )

      INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
      LINK_LIBRARIES( ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${CMAKE_THREAD_LIBS} )
      SET( OPENGL_LIBRARIES ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} ${CMAKE_THREAD_LIBS} )

    ENDIF(OPENGL_glu_LIBRARY)
    ENDIF(OPENGL_gl_LIBRARY)
    ENDIF(OPENGL_INCLUDE_PATH)
  
  ENDIF (WIN32)
ENDIF(NOT HAS_OPENGL)
