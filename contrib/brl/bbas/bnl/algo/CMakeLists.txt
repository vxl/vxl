SET( bnl_algo_sources
    bnl_legendre_polynomial.cxx       bnl_legendre_polynomial.h

                                           bnl_fst.h 
    bnl_fst_memo.cxx                       bnl_fst_memo.h
    bnl_shw_rotation_estimator.cxx    bnl_shw_rotation_estimator.h
    
   )

ADD_LIBRARY( bnl_algo ${bnl_algo_sources} )
INSTALL_TARGETS(/lib bnl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/bbas/bnl/algo ${bnl_algo_sources})
TARGET_LINK_LIBRARIES( bnl_algo bnl vnl vnl_algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
