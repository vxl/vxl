INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/b3p/netlib)

ADD_EXECUTABLE( b3p_netlib_test_all  b3p_test_integral.cxx)

ADD_TEST(b3p_test_integral b3p_netlib_test_all)

TARGET_LINK_LIBRARIES(b3p_netlib_test_all b3p_netlib vcl)
