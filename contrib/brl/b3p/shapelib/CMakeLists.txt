#This is brl/b3p/shapelib

PROJECT( shapelib C )

INCLUDE( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

IF(NOT VXL_USING_NATIVE_SHAPELIB)

IF (WIN32)
  ADD_DEFINITIONS( -DXML_STATIC )
  ADD_DEFINITIONS( -DCOMPILED_FROM_DSP )
ENDIF (WIN32)

#set the COMPILED_FROM_DSP definition


SET(shapelib_sources
   shapefil.h
   shputils.c
   shptreedump.c
   shptree.c
   shprewind.c
   shpopen.c
   shpdump.c
   shpcreate.c
   shpadd.c
   dbfopen.c
   dbfdump.c
   dbfcreate.c
   dbfadd.c
)

ADD_LIBRARY(shapelib ${shapelib_sources})
INSTALL_TARGETS(/lib shapelib)


ENDIF(NOT VXL_USING_NATIVE_SHAPELIB)

