
SOURCE_FILES( vcl_common
vcl_cmath
vcl_cassert
vcl_cstdlib
)

IF(WIN32)
  ADD_DEFINITIONS( -DBUILDING_VCL_DLL )
  INCLUDE_DIRECTORIES( config.win32-vc60 )
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(Templates vcl_common)

ADD_LIBRARY(vcl vcl_common)

#SUBDIRS(tests)

