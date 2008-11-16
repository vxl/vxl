INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/bvxm )

ADD_EXECUTABLE( bvxm_rec_test_all
  test_driver.cxx
  test_bvxm_densities.cxx
  test_param_estimation.cxx
  test_bvxm_parts.cxx
  test_bvxm_hierarchy_builder.cxx
)

TARGET_LINK_LIBRARIES( bvxm_rec_test_all bvxm_rec testlib vcl vbl vpgl )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/digits_small.TIF    ${CMAKE_CURRENT_BINARY_DIR}/digits_small.TIF COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_cropped.tif    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_cropped.tif COPYONLY )

ADD_TEST( bvxm_test_densities     ${EXECUTABLE_OUTPUT_PATH}/bvxm_rec_test_all test_bvxm_densities )
ADD_TEST( bvxm_test_param_estimation     ${EXECUTABLE_OUTPUT_PATH}/bvxm_rec_test_all test_bvxm_param_estimation )
ADD_TEST( bvxm_test_parts     ${EXECUTABLE_OUTPUT_PATH}/bvxm_rec_test_all test_bvxm_parts )
ADD_TEST( bvxm_test_hierarchy_builder     ${EXECUTABLE_OUTPUT_PATH}/bvxm_rec_test_all test_bvxm_hierarchy_builder )

ADD_EXECUTABLE( bvxm_rec_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_rec_test_include bvxm_rec )
