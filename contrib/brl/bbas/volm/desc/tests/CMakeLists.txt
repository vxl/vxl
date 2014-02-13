# contrib/brl/bbas/volm/desc/tests/CMakeLists.txt

ADD_EXECUTABLE( volm_desc_test_all
  test_driver.cxx
  test_volm_descriptor.cxx
)

TARGET_LINK_LIBRARIES( volm_desc_test_all volm_desc testlib )
ADD_TEST( volm_desc_test_descriptor ${EXECUTABLE_OUTPUT_PATH}/volm_desc_test_all test_volm_descriptor )

ADD_EXECUTABLE( volm_desc_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( volm_desc_test_include volm_desc )
