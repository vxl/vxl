#
# try to find X11 on UNIX systems.
#

IF(NOT HAS_X11)


  IF (UNIX)
    
    FIND_PATH(X11_INCLUDE_PATH X11/X.h
      /usr/include 
      /usr/local/include 
      /usr/openwin/include 
      /usr/openwin/share/include 
      /usr/X11R6/include 
      /usr/include/X11
    )
  
  
    FIND_LIBRARY(X11_X11_LIBRARY X11
      /usr/lib 
      /usr/local/lib 
      /usr/openwin/lib 
      /usr/X11R6/lib
    )
  
    FIND_LIBRARY(X11_Xext_LIBRARY Xext
      /usr/lib 
      /usr/local/lib 
      /usr/openwin/lib 
      /usr/X11R6/lib
    )
  
    IF(X11_INCLUDE_PATH)
      IF(X11_X11_LIBRARY)
  
        SET( HAS_X11 "YES" )
        ADD_DEFINITIONS(-DHAS_X11)

        SET( X11_LIBRARIES ${X11_X11_LIBRARY} )
  
      ENDIF(X11_X11_LIBRARY)
  
      IF(X11_Xext_LIBRARY)
        SET( X11_LIBRARIES ${X11_LIBRARIES} ${X11_Xext_LIBRARY} )
      ENDIF(X11_Xext_LIBRARY)
    ENDIF(X11_INCLUDE_PATH)

  ENDIF (UNIX)
ENDIF(NOT HAS_X11)
