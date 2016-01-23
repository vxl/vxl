#This is brl/b3p/shapelib

project( shapelib C )

include( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

if(NOT VXL_USING_NATIVE_SHAPELIB)

  if(WIN32)
    add_definitions( -DXML_STATIC )
    add_definitions( -DCOMPILED_FROM_DSP )
  endif(WIN32)

  #set the COMPILED_FROM_DSP definition

  set(shapelib_sources
     shapefil.h
     shptree.c
     shpopen.c
     dbfopen.c
  )

  add_library(shapelib ${shapelib_sources})
  install_targets(/lib shapelib)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

endif(NOT VXL_USING_NATIVE_SHAPELIB)

