project(expat C)
#include_regular_expression("^.*$")

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(NOT VXL_USING_NATIVE_EXPAT)

  if(WIN32)
    if(NOT BUILD_SHARED_LIBS)
      set(CM_EXPAT_STATIC 1)
    endif()
  endif()

  vxl_configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/expatConfig.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/expatConfig.h
      include/vxl/contrib/brl/b3p/expat
  )
  vxl_configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/expatDllConfig.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/expatDllConfig.h
      include/vxl/contrib/brl/b3p/expat
  )

  configure_file(${expat_SOURCE_DIR}/.NoDartCoverage
                 ${expat_BINARY_DIR}/.NoDartCoverage)

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
    )

  include(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
  TEST_BIG_ENDIAN(expat_BIGENDIAN)

  vxl_add_library(
      LIBRARY_NAME expat
      HEADER_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}"
      LIBRARY_SOURCES ${expat_sources}
      DISABLE_MSVC_MP
  )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
