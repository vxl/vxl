#
# ./v3p/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


SUBDIRS(netlib)
SUBDIRS(Qv)

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeJPEG.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeTIFF.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativePNG.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeZLIB.cmake)

IF(NOT HAS_NATIVE_JPEG)
  SUBDIRS(jpeg)
ENDIF(NOT HAS_NATIVE_JPEG)

IF(NOT HAS_NATIVE_TIFF)
  SUBDIRS(tiff)
ENDIF(NOT HAS_NATIVE_TIFF)

IF(NOT USE_NATIVE_PNG)
  SUBDIRS(png)
ENDIF(NOT USE_NATIVE_PNG)

IF(NOT HAS_NATIVE_ZLIB)
  SUBDIRS(zlib)
ENDIF(NOT HAS_NATIVE_ZLIB)

