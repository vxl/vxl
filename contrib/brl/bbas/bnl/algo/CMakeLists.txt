SET( bnl_algo_sources
    bnl_legendre_polynomial.cxx       bnl_legendre_polynomial.h
   )

ADD_LIBRARY( bnl_algo ${bnl_algo_sources} )
TARGET_LINK_LIBRARIES( bnl_algo bnl vnl vnl_algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
