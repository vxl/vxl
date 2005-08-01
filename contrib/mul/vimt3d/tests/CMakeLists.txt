# This is mul/vimt3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt3d_test_all
  test_driver.cxx
  test_transform_3d.cxx
  test_image_3d_of.cxx
  test_load.cxx
  test_sample_grid_trilin.cxx
  test_v3i.cxx
  test_from_image_2d.cxx
  test_resample.cxx
  test_gauss_reduce.cxx
)

TARGET_LINK_LIBRARIES( vimt3d_test_all vimt3d vpl vul testlib )

ADD_TEST( vimt3d_test_transform_3d         vimt3d_test_all test_transform_3d )
ADD_TEST( vimt3d_test_image_3d_of          vimt3d_test_all test_image_3d_of )
ADD_TEST( vimt3d_test_load                 vimt3d_test_all test_load ${mul_SOURCE_DIR}/vil3d/tests/file_read_data)
ADD_TEST( vimt3d_test_sample_grid_trilin   vimt3d_test_all test_sample_grid_trilin )
ADD_TEST( vimt3d_test_v3i                  vimt3d_test_all test_v3i )
ADD_TEST( vimt3d_test_from_image_2d        vimt3d_test_all test_from_image_2d )
ADD_TEST( vimt3d_test_gauss_reduce         vimt3d_test_all test_gauss_reduce )
ADD_TEST( vimt3d_test_resample             vimt3d_test_all test_resample )

ADD_EXECUTABLE( vimt3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vimt3d_test_include vimt3d )

