# mul/mipa/tests/CMakeLists.txt

add_executable( mipa_test_all
  test_driver.cxx

  test_sample_histo_boxes.cxx
  test_vector_normalisers.cxx
)
target_link_libraries( mipa_test_all mipa ${VXL_LIB_PREFIX}testlib )

add_test( NAME mipa_test_sample_histo_boxes COMMAND $<TARGET_FILE:mipa_test_all>
test_sample_histo_boxes)
add_test( NAME mipa_test_vector_normalisers COMMAND $<TARGET_FILE:mipa_test_all> test_vector_normalisers)

add_executable( mipa_test_include test_include.cxx )
target_link_libraries( mipa_test_include mipa )
add_executable( mipa_test_template_include test_template_include.cxx )
target_link_libraries( mipa_test_template_include mipa )
