#
# try to find X11 on UNIX systems.
#

IF (UNIX)
  
  FIND_PATH(X11_INCLUDE_PATH X11/X.h
  /usr/include 
  /usr/local/include 
  /usr/openwin/include 
  /usr/openwin/share/include 
  /usr/X11R6/include 
  )


  FIND_LIBRARY(X11_LIB_PATH X11
  /usr/lib 
  /usr/local/lib 
  /usr/openwin/lib 
  /usr/X11R6/lib
  )

  IF(X11_INCLUDE_PATH)
    SET (X11_LIBRARY "-lXext -lX11" CACHE)
  ENDIF(X11_INCLUDE_PATH)

ENDIF (UNIX)
