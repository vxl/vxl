# This is mul/vimt3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt3d_test_all
  test_driver.cxx

  test_transform_3d.cxx
  test_image_3d_of.cxx
)

TARGET_LINK_LIBRARIES( vimt3d_test_all vimt3d vpl vul testlib )

ADD_TEST( vimt3d_test_transform_3d  vimt3d_test_all test_transform_3d )
ADD_TEST( vimt3d_test_image_3d_of   vimt3d_test_all test_image_3d_of )

ADD_EXECUTABLE( vimt3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vimt3d_test_include vimt3d )
