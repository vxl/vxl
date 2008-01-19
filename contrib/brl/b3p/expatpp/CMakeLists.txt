#This is brl/b3p/expatpp

OPTION(BUILD_EXPAT "Build expatpp XML parser" NO)
IF (BUILD_BGUI3D)
IF (WIN32)
  ADD_DEFINITIONS( -DXML_STATIC )
  ADD_DEFINITIONS( -DCOMPILED_FROM_DSP )
ENDIF (WIN32)

#set the COMPILED_FROM_DSP definition

IF (APPLE)
  SET(expatpp_sources expatpp.h expatpp.cpp)
ELSE (APPLE)
 SET(expatpp_sources
    ascii.h              asciitab.h
    expat.h              iasciitab.h
    internal.h           latin1tab.h
    macconfig.h          nametab.h
    utf8tab.h            winconfig.h
    xmlparse.c xmlrole.c xmlrole.h
    xmltok.c xmltok.h    xmltok_impl.h
    expatpp.cpp          expatpp.h expatpplib.h
 )
ENDIF (APPLE)

  ADD_LIBRARY(expatpp ${expatpp_sources})

IF (APPLE)
  TARGET_LINK_LIBRARIES(expatpp expat)
ELSE (APPLE)
  TARGET_LINK_LIBRARIES(expatpp )
ENDIF (APPLE)

ENDIF (BUILD_BGUI3D)
