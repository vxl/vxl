# mul/mbl/tests/CMakeLists.txt

TARGET_LINK_LIBRARIES( test_mbl mbl vbl vul testlib )

ADD_EXECUTABLE( test_mbl test_mbl.cxx )
ADD_TEST(   mbl_test_mbl test_mbl     )
