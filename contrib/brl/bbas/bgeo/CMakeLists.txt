# Brown geospatial library
# brl/bbas/bgeo/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bgeo_sources
  bgeo_lvcs.cxx          bgeo_lvcs.h           bgeo_lvcs_sptr.h
  bgeo_datum_conversion.cxx  bgeo_datum_conversion.h
                             bgeo_earth_constants.h

  bgeo_dll.h
)
AUX_SOURCE_DIRECTORY(Templates bgeo_sources)

ADD_LIBRARY(bgeo ${bgeo_sources})
INSTALL_TARGETS(/lib bgeo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bgeo ${bgeo_sources})

TARGET_LINK_LIBRARIES(bgeo vnl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
