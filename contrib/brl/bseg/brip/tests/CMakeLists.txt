# brl/bseg/brip/tests/CMakeLists.txt


ADD_EXECUTABLE( brip_test_all
  test_driver.cxx
  test_histogram.cxx
  test_mutual_info.cxx
)

TARGET_LINK_LIBRARIES( brip_test_all brip testlib vil)

ADD_TEST( brip_test_histogram         brip_test_all test_histogram )
ADD_TEST( brip_test_mutual_info       brip_test_all test_mutual_info )

ADD_EXECUTABLE( brip_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brip_test_include brip )
