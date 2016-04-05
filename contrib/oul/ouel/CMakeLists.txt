# ./oul/ouel/CMakeLists.txt

doxygen_add_library(contrib/oul/ouel
  DEPENDS core/vnl
  PACKAGE contrib/oul
  DESCRIPTION "Estimation/filtering library"
  )

set(ouel_sources
  kalman_filter.h    kalman_filter.cxx
)

vxl_add_library(LIBRARY_NAME ouel LIBRARY_SOURCES ${ouel_sources})
target_link_libraries( ouel ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
