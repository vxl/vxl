#
# try to find GTK (and glib) and GTKGLArea
#

SET( HAS_GTK "NO" )

# don't even bother under WIN32
IF (UNIX)

    FIND_PATH( GTK_gtk_INCLUDE_PATH gtk/gtk.h
      /usr/include
      /usr/local/include
      /usr/openwin/share/include
      /usr/openwin/include
      /usr/X11R6/include
      /usr/include/X11
    )

    FIND_PATH( GTK_glib_INCLUDE_PATH glibconfig.h
      /usr/include
      /usr/local/include
      /usr/openwin/share/include
    )

    FIND_PATH( GTK_gtkgl_INCLUDE_PATH gtkgl/gtkglarea.h
      /usr/include
      /usr/local/include
      /usr/openwin/share/include
    )

    FIND_LIBRARY( GTK_gtkgl_LIBRARY gtkgl
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GTK_gtk_LIBRARY gtk
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GTK_gdk_LIBRARY gdk
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GTK_gmodule_LIBRARY gmodule
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GTK_glib_LIBRARY glib
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )


    IF(GTK_gtk_INCLUDE_PATH)
    IF(GTK_glib_INCLUDE_PATH)
    IF(GTK_gtkgl_INCLUDE_PATH)
    IF(GTK_gtk_LIBRARY)
    IF(GTK_glib_LIBRARY)
    IF(GTK_gtkgl_LIBRARY)

      # Assume that if gtk and glib were found, the other 
      # supporting libraries have also been found.

      SET( HAS_GTK "YES" )
      ADD_DEFINITIONS( -DHAS_GTK )
      INCLUDE_DIRECTORIES( ${GTK_gtk_INCLUDE_PATH}
                           ${GTK_glib_INCLUDE_PATH}
                           ${GTK_gtkgl_INCLUDE_PATH} )
      LINK_LIBRARIES( ${GTK_gtkgl_LIBRARY}
                      ${GTK_gtk_LIBRARY}
                      ${GTK_gdk_LIBRARY}
                      ${GTK_gmodule_LIBRARY}
                      ${GTK_glib_LIBRARY}    )

    ENDIF(GTK_gtkgl_LIBRARY)
    ENDIF(GTK_glib_LIBRARY)
    ENDIF(GTK_gtk_LIBRARY)
    ENDIF(GTK_gtkgl_INCLUDE_PATH)
    ENDIF(GTK_glib_INCLUDE_PATH)
    ENDIF(GTK_gtk_INCLUDE_PATH)

ENDIF (UNIX)

