# v3p/jpeg/CMakeLists.txt

PROJECT( jpeg C )

INCLUDE( ${MODULE_PATH}/FindJPEG.cmake )

IF(NOT VXL_USING_NATIVE_JPEG)

SET( jpeg_sources
  jconfig.h
  jdct.h
  jinclude.h
  jmemsys.h
  jmorecfg.h
  jpegint.h
  jpeglib.h
  jversion.h

  jcapimin.c
  jcapistd.c
  jccoefct.c
  jccolor.c
  jcdctmgr.c
  jchuff.c jchuff.h
  jcinit.c
  jcmainct.c
  jcmarker.c
  jcmaster.c
  jcomapi.c
  jcparam.c
  jcphuff.c
  jcprepct.c
  jcsample.c
  jctrans.c
  jdapimin.c
  jdapistd.c
  jdatadst.c
  jdatasrc.c
  jdcoefct.c
  jdcolor.c
  jddctmgr.c
  jdhuff.c jdhuff.h
  jdinput.c
  jdmainct.c
  jdmarker.c
  jdmaster.c
  jdmerge.c
  jdphuff.c
  jdpostct.c
  jdsample.c
  jdtrans.c
  jerror.c jerror.h
  jfdctflt.c
  jfdctfst.c
  jfdctint.c
  jidctflt.c
  jidctfst.c
  jidctint.c
  jidctred.c
  jquant1.c
  jquant2.c
  jutils.c
  jmemmgr.c
  jmemansi.c
)

ADD_LIBRARY( jpeg ${jpeg_sources} )

ENDIF(NOT VXL_USING_NATIVE_JPEG)
