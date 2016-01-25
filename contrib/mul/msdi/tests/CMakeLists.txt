# mul/msdi/tests/CMakeLists.txt

ADD_EXECUTABLE( msdi_test_all
  test_driver.cxx
  test_reflected_marked_images.cxx
)
TARGET_LINK_LIBRARIES( msdi_test_all msdi testlib )

ADD_TEST( NAME msdi_test_reflected_marked_images COMMAND $<TARGET_FILE:msdi_test_all> test_reflected_marked_images)

ADD_EXECUTABLE( msdi_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msdi_test_include msdi )
