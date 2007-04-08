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

TARGET_LINK_LIBRARIES( brip_test_all brip vgl_algo vgl vnl_algo vnl vil vil1 testlib)

ADD_TEST( brip_test_histogram    ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_histogram )
ADD_TEST( brip_test_mutual_info  ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_mutual_info )
ADD_TEST( brip_test_watershed    ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_watershed )
ADD_TEST( brip_test_fourier           ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_fourier )
ADD_TEST( brip_test_homography        ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_homography )
ADD_TEST( brip_test_lucas_kanade      ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_lucas_kanade )
ADD_TEST( brip_test_Horn_Schunck      ${EXECUTABLE_OUTPUT_PATH}/brip_test_all test_Horn_Schunck )

ADD_EXECUTABLE( brip_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brip_test_include brip )
