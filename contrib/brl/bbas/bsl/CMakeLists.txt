# Brown Subjective Logic Library
# brl/bbas/bsl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bsl
  DEPENDS core/vul core/vbl
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )

set(bsl_sources
  bsl_opinion.h
  bsl_opinion.cxx
)
aux_source_directory(Templates bsl_sources)

add_library(bsl ${bsl_sources})
install_targets(/lib bsl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsl ${bsl_sources})
target_link_libraries(bsl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
