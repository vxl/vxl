# This is mul/vimt3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt3d_test_driver
  test_driver.cxx

  test_transform_3d.cxx
	test_image_3d_of.cxx
)
TARGET_LINK_LIBRARIES( vimt3d_test_driver vimt3d vpl vul testlib )

ADD_TEST( vimt3d_test_transform_3d         vimt3d_test_driver test_transform_3d )
ADD_TEST( vimt3d_test_image_3d_of          vimt3d_test_driver test_image_3d_of )
