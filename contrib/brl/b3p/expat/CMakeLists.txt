#This is brl/b3p/expat

PROJECT( expat C )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(NOT VXL_USING_NATIVE_EXPAT)

IF (WIN32)
  ADD_DEFINITIONS( -DXML_STATIC )
  ADD_DEFINITIONS( -DCOMPILED_FROM_DSP )
ENDIF (WIN32)

#set the COMPILED_FROM_DSP definition


SET(expat_sources
    ascii.h              asciitab.h
    expat.h              iasciitab.h
    internal.h           latin1tab.h
    macconfig.h          nametab.h
    utf8tab.h            winconfig.h
    xmlparse.c xmlrole.c xmlrole.h
    xmltok.c xmltok.h    xmltok_impl.h
)

ADD_LIBRARY(expat ${expat_sources})
INSTALL_TARGETS(/lib expat)


ENDIF(NOT VXL_USING_NATIVE_EXPAT)

