LINK_LIBRARIES( netlib )

INCLUDE(${MODULE_PATH}/NewCMake/FindX11.cmake

IF(X11_FOUND)

  ADD_EXECUTABLE( netlib_showme showme.c -lX11 )

ENDIF(X11_FOUND)
