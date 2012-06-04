# Brown Subjective Logic Library
# brl/bbas/bsl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bsl
  DEPENDS core/vul core/vbl
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bsl_sources
  bsl_opinion.h 
  bsl_opinion.cxx
)
AUX_SOURCE_DIRECTORY(Templates bsl_sources)

ADD_LIBRARY(bsl ${bsl_sources})
INSTALL_TARGETS(/lib bsl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsl ${bsl_sources})
TARGET_LINK_LIBRARIES(bsl vnl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
