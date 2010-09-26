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
     shptree.c
     shpopen.c
     dbfopen.c
  )

  ADD_LIBRARY(shapelib ${shapelib_sources})
  INSTALL_TARGETS(/lib shapelib)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(NOT VXL_USING_NATIVE_SHAPELIB)

