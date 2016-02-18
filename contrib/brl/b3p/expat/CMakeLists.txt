project(expat C)
#include_regular_expression("^.*$")

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(NOT VXL_USING_NATIVE_EXPAT)

set(expat_sources
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

include(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(expat_BIGENDIAN)

include_directories(${expat_BINARY_DIR})

if(WIN32)
  if(NOT VXL_BUILD_SHARED_LIBS)
    set(CM_EXPAT_STATIC 1)
  endif()
endif()

configure_file(${expat_SOURCE_DIR}/expatConfig.h.in ${expat_BINARY_DIR}/expatConfig.h)
configure_file(${expat_SOURCE_DIR}/expatDllConfig.h.in ${expat_BINARY_DIR}/expatDllConfig.h)
configure_file(${expat_SOURCE_DIR}/.NoDartCoverage ${expat_BINARY_DIR}/.NoDartCoverage)
configure_file(${expat_SOURCE_DIR}/expat.h ${expat_BINARY_DIR}/expat.h)

vxl_add_library( LIBNAME expat LIBSRCS ${expat_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
