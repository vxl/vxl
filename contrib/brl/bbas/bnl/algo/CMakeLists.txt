

SET( bnl_algo_sources
    bnl_legendre_polynomial.cxx       bnl_legendre_polynomial.h
    bnl_simpson_integral.cxx          bnl_simpson_integral.h
    bnl_adaptsimpson_integral.cxx     bnl_adaptsimpson_integral.h
    )

ADD_LIBRARY( bnl_algo ${bnl_algo_sources} )
TARGET_LINK_LIBRARIES( bnl_algo bnl vnl vnl_algo b3p_netlib)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

