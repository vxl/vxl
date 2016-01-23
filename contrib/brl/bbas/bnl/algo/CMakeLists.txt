# brl/bbas/bnl/algo/CMakeLists.txt

set( bnl_algo_sources
   bnl_legendre_polynomial.cxx      bnl_legendre_polynomial.h
   bnl_fresnel.cxx                  bnl_fresnel.h
)

add_library( bnl_algo ${bnl_algo_sources} )
install_targets(/lib bnl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bnl/algo ${bnl_algo_sources})
target_link_libraries( bnl_algo vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

