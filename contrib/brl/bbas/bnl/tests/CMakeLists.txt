SET( bnl_test_src 
                          test_bnl_fst.cxx
   )

ADD_EXECUTABLE( test_bnl ${bnl_test_src} )

TARGET_LINK_LIBRARIES( test_bnl vcl bnl)
