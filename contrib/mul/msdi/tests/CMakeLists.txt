# mul/msdi/tests/CMakeLists.txt

add_executable( msdi_test_all
  test_driver.cxx
  test_marked_images_from_files.cxx
  test_reflected_marked_images.cxx
)
target_link_libraries( msdi_test_all msdi ${VXL_LIB_PREFIX}testlib )

add_test( NAME msdi_test_reflected_marked_images COMMAND $<TARGET_FILE:msdi_test_all> test_reflected_marked_images)
add_test( NAME test_marked_images_from_files COMMAND $<TARGET_FILE:msdi_test_all> test_marked_images_from_files)

add_executable( msdi_test_include test_include.cxx )
target_link_libraries( msdi_test_include msdi )
