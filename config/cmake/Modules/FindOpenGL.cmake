#
# try to find OpenGL on UNIX systems once done this will define
#
# OPENGL_INCLUDE_PATH = where the GL include directory can be found
# OPENGL_LIB_PATH     = where the GL library can be found
# OPENGL_LIBRARY      = the name of the OpenGL library to link with
#
# If the variable OPENGL_LIBRARY is set, we assume that OpenGL is
# present. We cannot use OPENGL_INCLUDE_PATH, because sometimes the
# library is present but there isn't a special include path. (E.g. on
# Windows)
#

IF (WIN32)
  SET (OPENGL_LIBRARY opengl32 CACHE)
ELSE (WIN32)
  
  FIND_PATH(OPENGL_INCLUDE_PATH GL/gl.h 
  /usr/include 
  /usr/local/include 
  /usr/openwin/share/include 
  /opt/graphics/OpenGL/include 
  /usr/X11R6/include 
  )


  FIND_LIBRARY(OPENGL_LIB_PATH GL
  /usr/lib 
  /usr/local/lib 
  /opt/graphics/OpenGL/lib 
  /usr/openwin/lib 
  /usr/X11R6/lib
  )

  # right now we only look for -lgl maybe in future also mesa
  IF(OPENGL_INCLUDE_PATH)
    SET (OPENGL_LIBRARY "-lGL -lGLU" CACHE)
  ENDIF(OPENGL_INCLUDE_PATH)

ENDIF (WIN32)
