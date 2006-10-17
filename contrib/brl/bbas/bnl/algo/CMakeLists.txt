SET( bnl_algo_sources
    bnl_legendre_polynomial.cxx       bnl_legendre_polynomial.h
   )

ADD_LIBRARY( bnl_algo ${bnl_algo_sources} )
INSTALL_TARGETS(/lib bnl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bnl/algo ${bnl_algo_sources})
TARGET_LINK_LIBRARIES( bnl_algo vnl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
