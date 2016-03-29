# brl/bbas/bnl/algo/CMakeLists.txt

set( bnl_algo_sources
   bnl_legendre_polynomial.cxx      bnl_legendre_polynomial.h
   bnl_fresnel.cxx                  bnl_fresnel.h
)

vxl_add_library(LIBRARY_NAME bnl_algo LIBRARY_SOURCES ${bnl_algo_sources} )
target_link_libraries( bnl_algo ${VXL_LIB_PREFIX}vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

