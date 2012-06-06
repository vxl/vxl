# mul/mipa/tests/CMakeLists.txt

ADD_EXECUTABLE( mipa_test_all
  test_driver.cxx

  test_sample_histo_boxes.cxx
  test_vector_normalisers.cxx
)
TARGET_LINK_LIBRARIES( mipa_test_all mipa testlib )

ADD_TEST( mipa_test_sample_histo_boxes ${EXECUTABLE_OUTPUT_PATH}/mipa_test_all
test_sample_histo_boxes)
ADD_TEST( mipa_test_vector_normalisers ${EXECUTABLE_OUTPUT_PATH}/mipa_test_all test_vector_normalisers)

ADD_EXECUTABLE( mipa_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mipa_test_include mipa )
ADD_EXECUTABLE( mipa_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( mipa_test_template_include mipa )
