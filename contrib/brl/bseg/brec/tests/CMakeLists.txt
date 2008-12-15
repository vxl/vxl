INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/brec )

ADD_EXECUTABLE( brec_test_all
  test_driver.cxx
  test_brec_densities.cxx
  test_param_estimation.cxx
  test_brec_parts.cxx
  test_brec_hierarchy_builder.cxx
  test_brec_hierarchy_detector.cxx
)

TARGET_LINK_LIBRARIES( brec_test_all brec testlib vcl vbl vpgl )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/digits_small.TIF    ${CMAKE_CURRENT_BINARY_DIR}/digits_small.TIF COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_cropped.tif    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_cropped.tif COPYONLY )

ADD_TEST( brec_test_densities     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_densities )
ADD_TEST( brec_test_param_estimation     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_param_estimation )
ADD_TEST( brec_test_parts     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_parts )
ADD_TEST( brec_test_hierarchy_builder     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_hierarchy_builder )
ADD_TEST( brec_test_hierarchy_detector     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_hierarchy_detector )

ADD_EXECUTABLE( brec_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brec_test_include brec )
