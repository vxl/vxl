
SOURCE_FILES( vcl_common
vcl_cmath
vcl_cassert
vcl_cstdlib
)

WIN32_DEFINES( -DBUILDING_VCL_DLL )
WIN32_INCLUDE_DIRECTORIES( config.win32-vc60 )

AUX_SOURCE_DIRECTORY(Templates vcl_common)

ADD_LIBRARY(vcl vcl_common)

#SUBDIRS(tests)

