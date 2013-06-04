# contrib/brl/bseg/boxm2/volm/desc/tests/CMakeLists.txt

ADD_EXECUTABLE( boxm2_volm_desc_test_all
  test_driver.cxx
  test_boxm2_volm_desc_ex_indexer.cxx
)

TARGET_LINK_LIBRARIES( boxm2_volm_desc_test_all boxm2_volm_desc testlib )

ADD_TEST( boxm2_volm_desc_test_ex_indexer ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_desc_test_all test_ex_indexer )

ADD_EXECUTABLE( boxm2_volm_desc_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_desc_test_include boxm2_volm_desc )