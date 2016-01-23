#
# try to find glew (OpenGL Extensions Wrangler Libarary) libarary and include files
#
# GLEW_INCLUDE_DIR, where to find GL/glew.h, etc.
# GLEW_LIBRARIES, the libraries to link against to use GLEW.
# GLEW_FOUND, If false, do not try to use GLEW.


if(WIN32)

  if(CYGWIN)

    find_path( GLEW_INCLUDE_DIR GL/glew.h
      ${OPENGL_INCLUDE_DIR}
    )

    find_library( GLEW_LIBRARY glew32
      ${OPENGL_LIBRARY_DIR}
      /usr/lib64
      /usr/lib
      /usr/lib/w32api
      /usr/local/lib
      /usr/X11R6/lib
    )


  else()

    find_path( GLEW_INCLUDE_DIR GL/glew.h
      ${OPENGL_INCLUDE_DIR}
    )

    find_library( GLEW_LIBRARY glew64
      ${OPENGL_LIBRARY_DIR}
    )

  endif()

else()

    find_path( GLEW_INCLUDE_DIR GL/glew.h
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

    find_library( GLEW_LIBRARY
      NAMES glew GLEW
      PATHS
            /usr/lib64
            /usr/lib
            /usr/local/lib
            /usr/openwin/lib
            /usr/X11R6/lib
            ${OPENGL_LIBRARY_DIR}
    )

endif()

set( GLEW_FOUND "NO" )
if(GLEW_INCLUDE_DIR)
  if(GLEW_LIBRARY)
    set( GLEW_FOUND "YES" )
  endif()
endif()

mark_as_advanced(
  GLEW_INCLUDE_DIR
  GLEW_LIBRARY
)
