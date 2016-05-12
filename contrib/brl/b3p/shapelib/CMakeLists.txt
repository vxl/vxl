#This is brl/b3p/shapelib

project( shapelib C )

include( ${BRL_MODULE_PATH}/FindSHAPELIB.cmake )

if(NOT VXL_USING_NATIVE_SHAPELIB)

  if(WIN32)
    add_definitions( -DXML_STATIC )
    add_definitions( -DCOMPILED_FROM_DSP )
  endif()

  #set the COMPILED_FROM_DSP definition

  set(shapelib_sources
     shapefil.h
     shptree.c
     shpopen.c
     dbfopen.c
  )

  vxl_add_library(LIBRARY_NAME shapelib LIBRARY_SOURCES ${shapelib_sources})

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()

