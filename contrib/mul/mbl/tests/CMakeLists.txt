#
# ./mul/mbl/tests/CMakeLists.txt
#
#   Ian Scott
#

ADD_EXECUTABLE( test_mbl test_mbl.cxx )
TARGET_LINK_LIBRARIES( test_mbl mbl vbl vul )

ADD_TEST(mbl_test_mbl test_mbl)
