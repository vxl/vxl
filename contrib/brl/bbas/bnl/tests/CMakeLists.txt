ADD_EXECUTABLE( bnl_test_all  test_bnl_all.cxx )

ADD_TEST(bnl_test_all bnl_test_all)

TARGET_LINK_LIBRARIES(bnl_test_all bnl vnl)

