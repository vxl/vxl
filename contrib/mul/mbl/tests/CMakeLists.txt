# mul/mbl/tests/CMakeLists.txt

LINK_LIBRARIES( mbl vbl vul vpl testlib )

ADD_EXECUTABLE( test_mbl test_mbl.cxx )
ADD_TEST(   mbl_test_mbl test_mbl     )
