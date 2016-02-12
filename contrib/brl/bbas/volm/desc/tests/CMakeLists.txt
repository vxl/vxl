# contrib/brl/bbas/volm/desc/tests/CMakeLists.txt

add_executable( volm_desc_test_all
  test_driver.cxx
  test_volm_descriptor.cxx
)

target_link_libraries( volm_desc_test_all volm_desc ${VXL_LIB_PREFIX}testlib )
add_test( NAME volm_desc_test_descriptor COMMAND $<TARGET_FILE:volm_desc_test_all> test_volm_descriptor )

add_executable( volm_desc_test_include test_include.cxx )
target_link_libraries( volm_desc_test_include volm_desc )
