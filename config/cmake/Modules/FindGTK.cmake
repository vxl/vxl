#
# try to find GTK
#

# don't even bother under WIN32
IF (UNIX)

  FIND_PATH(GTK_INCLUDE_PATH gtk/gtk.h
  /usr/include
  /usr/local/include
  /usr/openwin/share/include
  )

  FIND_LIBRARY(GTK_LIB_PATH gtk
  /usr/lib
  /usr/local/lib
  /usr/openwin/lib
  /usr/X11R6/lib
  )

  FIND_LIBRARY(GTK_GLIB_INCLUDE_PATH glibconfig.h
  /usr/include
  /usr/local/include
  /usr/openwin/share/include
  )

  IF(GTK_INCLUDE_PATH)
    SET (GTK_LIBRARY "-lgtk -lgdk -lgmodule -lglib" CACHE)
  ENDIF(GTK_INCLUDE_PATH)

ENDIF (UNIX)

