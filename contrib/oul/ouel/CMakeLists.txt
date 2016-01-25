# ./oul/ouel/CMakeLists.txt

doxygen_add_library(contrib/oul/ouel
  DEPENDS core/vnl
  PACKAGE contrib/oul
  DESCRIPTION "Estimation/filtering library"
  )

set(ouel_sources
  kalman_filter.h    kalman_filter.cxx
)

add_library(ouel ${ouel_sources})
install_targets(/lib ouel)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul/ouel ${ouel_sources})
target_link_libraries( ouel vnl_algo vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
