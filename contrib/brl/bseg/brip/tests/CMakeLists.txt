# brl/bseg/brip/tests/CMakeLists.txt

ADD_EXECUTABLE( brip_test_all
  test_driver.cxx
  test_histogram.cxx
  test_mutual_info.cxx
  test_watershed.cxx
  test_fourier.cxx
  test_homography.cxx
  test_lucas_kanade.cxx
  test_Horn_Schunck.cxx
)

TARGET_LINK_LIBRARIES( brip_test_all brip testlib vgl vil)

ADD_TEST( brip_test_histogram         brip_test_all test_histogram )
ADD_TEST( brip_test_mutual_info       brip_test_all test_mutual_info )
ADD_TEST( brip_test_watershed         brip_test_all test_watershed )
ADD_TEST( brip_test_fourier           brip_test_all test_fourier )
ADD_TEST( brip_test_homography        brip_test_all test_homography )
ADD_TEST( brip_test_lucas_kanade      brip_test_all test_lucas_kanade )
ADD_TEST( brip_test_Horn_Schunck      brip_test_all test_Horn_Schunck )

ADD_EXECUTABLE( brip_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brip_test_include brip )
