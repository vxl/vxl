#
# ./v3p/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


SUBDIRS(netlib)

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeJPEG.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeTIFF.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativePNG.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeZLIB.cmake)

IF(NOT HAS_NATIVE_JPEG)
  SUBDIRS(jpeg)
ENDIF(NOT HAS_NATIVE_JPEG)

IF(NOT NATIVE_TIFF_LIBRARY)
  SUBDIRS(tiff)
ENDIF(NOT NATIVE_TIFF_LIBRARY)

IF(NOT NATIVE_PNG_LIBRARY)
  SUBDIRS(png)
ENDIF(NOT NATIVE_PNG_LIBRARY)

IF(NOT NATIVE_ZLIB_LIBRARY)
  SUBDIRS(zlib)
ENDIF(NOT NATIVE_ZLIB_LIBRARY)

