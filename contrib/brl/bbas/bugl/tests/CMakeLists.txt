# This brl/bbas/bugl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

ADD_EXECUTABLE ( bugl_test_all
  test_driver.cxx
  test_bugl.cxx
  test_random_transform_set_2d.cxx
)
TARGET_LINK_LIBRARIES( bugl_test_all bugl vgl vnl testlib )

ADD_TEST( bugl_test_bugl   ${EXECUTABLE_OUTPUT_PATH}/bugl_test_all test_bugl)
ADD_TEST( bugl_test_random_transform_2d ${EXECUTABLE_OUTPUT_PATH}/bugl_test_all test_random_transform_set_2d)

ADD_EXECUTABLE( bugl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bugl_test_include bugl)
