PROJECT(expat C)
#INCLUDE_REGULAR_EXPRESSION("^.*$")

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(NOT VXL_USING_NATIVE_EXPAT)

SET(expat_sources
    xmlparse.c
    xmltok.c        xmltok.h
    xmlrole.c       xmlrole.h
    xmltok_impl.h
    ascii.h
    asciitab.h
    iasciitab.h
    nametab.h
    latin1tab.h
    utf8tab.h
    expat.h
    cm_expat_mangle.h
    expatConfig.h.in
    expatDllConfig.h.in
   )

INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(expat_BIGENDIAN)

INCLUDE_DIRECTORIES(${expat_BINARY_DIR})

IF(WIN32)
  IF(NOT BUILD_SHARED_LIBS)
    SET (CM_EXPAT_STATIC 1)
  ENDIF(NOT BUILD_SHARED_LIBS)
ENDIF(WIN32)

CONFIGURE_FILE(${expat_SOURCE_DIR}/expatConfig.h.in ${expat_BINARY_DIR}/expatConfig.h)
CONFIGURE_FILE(${expat_SOURCE_DIR}/expatDllConfig.h.in ${expat_BINARY_DIR}/expatDllConfig.h)
CONFIGURE_FILE(${expat_SOURCE_DIR}/.NoDartCoverage ${expat_BINARY_DIR}/.NoDartCoverage)
CONFIGURE_FILE(${expat_SOURCE_DIR}/expat.h ${expat_BINARY_DIR}/expat.h)

ADD_LIBRARY(expat ${expat_sources})
INSTALL_TARGETS(/lib expat)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(NOT VXL_USING_NATIVE_EXPAT)
