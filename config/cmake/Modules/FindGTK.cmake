#
# try to find GTK (and glib) and GTKGLArea
#

IF(NOT HAS_GTK)
  # don't even bother under WIN32
  IF (UNIX)

      FIND_PATH( GTK_gtk_INCLUDE_PATH gtk/gtk.h
        /usr/include
        /usr/local/include
        /usr/openwin/share/include
        /usr/openwin/include
        /usr/X11R6/include
        /usr/include/X11
        /usr/X11R6/include/gtk12
        /usr/include/gtk-1.2
	/usr/local/include/gtk-1.2
      )

      # Some Linux distributions (e.g. Red Hat) have glibconfig.h
      # and glib.h in different directories, so we need to look
      # for both.
      #  - Atanas Georgiev <atanas@cs.columbia.edu>

      FIND_PATH( GTK_glibconfig_INCLUDE_PATH glibconfig.h
        /usr/include
        /usr/local/include
        /usr/openwin/share/include
        /usr/local/include/glib12
        /usr/lib/glib/include
	/usr/local/lib/glib/include
      )

      FIND_PATH( GTK_glib_INCLUDE_PATH glib.h
        /usr/include
        /usr/local/include
        /usr/openwin/share/include
        /usr/include/gtk-1.2
        /usr/local/include/glib12
        /usr/lib/glib/include
        /usr/include/glib-1.2
	/usr/local/include/glib-1.2
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

      #
      # The 12 suffix is thanks to the FreeBSD ports collection
      #

      FIND_LIBRARY( GTK_gtk_LIBRARY
        NAMES  gtk gtk12
        PATHS /usr/lib
              /usr/local/lib
              /usr/openwin/lib
              /usr/X11R6/lib
      )

      FIND_LIBRARY( GTK_gdk_LIBRARY
        NAMES  gdk gdk12
        PATHS  /usr/lib
               /usr/local/lib
               /usr/openwin/lib
               /usr/X11R6/lib
      )

      FIND_LIBRARY( GTK_gmodule_LIBRARY
        NAMES  gmodule gmodule12
        PATHS  /usr/lib
               /usr/local/lib
               /usr/openwin/lib
               /usr/X11R6/lib
      )

      FIND_LIBRARY( GTK_glib_LIBRARY
        NAMES  glib glib12
        PATHS  /usr/lib
               /usr/local/lib
               /usr/openwin/lib
               /usr/X11R6/lib
      )

      IF(GTK_gtk_INCLUDE_PATH)
      IF(GTK_glibconfig_INCLUDE_PATH)
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
                             ${GTK_glibconfig_INCLUDE_PATH}
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
      ENDIF(GTK_glibconfig_INCLUDE_PATH)
      ENDIF(GTK_glib_INCLUDE_PATH)
      ENDIF(GTK_gtk_INCLUDE_PATH)

  ENDIF (UNIX)
ENDIF(NOT HAS_GTK)
