#This is contrib/brl/bbas/expatpp

IF (WIN32)
  ADD_DEFINITIONS( -DXML_STATIC )
  ADD_DEFINITIONS( -DCOMPILED_FROM_DSP )
ENDIF (WIN32)

#set the COMPILED_FROM_DSP definition

IF (APPLE)
  SET(expatpp_sources expatpp.h expatpp.cpp)
ELSE (APPLE)
 SET(expatpp_sources
    ascii.h asciitab.h
    expat.h iasciitab.h
    internal.h latin1tab.h
    macconfig.h nametab.h
    utf8tab.h winconfig.h
    xmlparse.c xmlrole.c 
##  libxmlrole.h
    xmltok.c xmltok.h
    expatpp.h expatpp.cpp
 )
ENDIF (APPLE)

  ADD_LIBRARY(expatpp ${expatpp_sources})

IF (APPLE)
  TARGET_LINK_LIBRARIES(expatpp expat)
ELSE (APPLE)
  TARGET_LINK_LIBRARIES(expatpp )
ENDIF (APPLE)

#SUBDIRS( SimpleTest )
