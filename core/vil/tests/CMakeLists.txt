# This is core/vil/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_test_all
  test_driver.cxx

  # basic things
  test_image_resource.cxx
  test_image_view.cxx
  test_memory_chunk.cxx
  test_pixel_format.cxx

  # file format readers/writers
  test_file_format_read.cxx
  test_save_load_image.cxx
  test_image_loader_robustness.cxx
  test_stream.cxx

  # image operations
  test_deep_copy_3_plane.cxx
  test_image_view_maths.cxx
  test_convert.cxx
  test_rotate_image.cxx
  test_warp.cxx

  # Sampling Operations
  test_bilin_interp.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_resample_bilin.cxx
  test_bicub_interp.cxx
  test_sample_profile_bicub.cxx
  test_sample_grid_bicub.cxx
  test_resample_bicub.cxx

  # Math
  test_math_value_range.cxx
)

TARGET_LINK_LIBRARIES( vil_test_all vil vpl vul testlib vcl )

# basic things
ADD_TEST( vil_test_image_resource           vil_test_all test_image_resource)
ADD_TEST( vil_test_image_view               vil_test_all test_image_view)
ADD_TEST( vil_test_memory_chunk             vil_test_all test_memory_chunk)
ADD_TEST( vil_test_pixel_format             vil_test_all test_pixel_format)

# file format readers/writers
ADD_TEST( vil_test_file_format_read         vil_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil_test_save_load_image          vil_test_all test_save_load_image)
ADD_TEST( vil_test_image_loader_robustness  vil_test_all test_image_loader_robustness)
ADD_TEST( vil_test_stream                   vil_test_all test_stream ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

# image operations
ADD_TEST( vil_test_deep_copy_3_plane        vil_test_all test_deep_copy_3_plane )
ADD_TEST( vil_test_image_view_maths         vil_test_all test_image_view_maths)
ADD_TEST( vil_test_convert                  vil_test_all test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil_test_rotate_image             vil_test_all test_rotate_image)
ADD_TEST( vil_test_warp                     vil_test_all test_warp)

# Sampling Operations
ADD_TEST( vil_test_bilin_interp             vil_test_all test_bilin_interp)
ADD_TEST( vil_test_sample_profile_bilin     vil_test_all test_sample_profile_bilin)
ADD_TEST( vil_test_sample_grid_bilin        vil_test_all test_sample_grid_bilin)
ADD_TEST( vil_test_resample_bilin           vil_test_all test_resample_bilin)
ADD_TEST( vil_test_bicub_interp             vil_test_all test_bicub_interp)
ADD_TEST( vil_test_sample_profile_bicub     vil_test_all test_sample_profile_bicub)
ADD_TEST( vil_test_sample_grid_bicub        vil_test_all test_sample_grid_bicub)
ADD_TEST( vil_test_resample_bicub           vil_test_all test_resample_bicub)

# Math
ADD_TEST( vil_test_math_value_range         vil_test_all test_math_value_range)


# To run timing tests
#ADD_EXECUTABLE( vil_test_access_timings vil_test_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings vil mbl vcl )
#ADD_TEST( vil_test_access_timings vil_test_access_timings )

#ADD_EXECUTABLE( vil_test_access_timings_noplanes vil_test_access_timings_noplanes.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings_noplanes vil mbl vcl )
#ADD_TEST( vil_test_access_timings_noplanes vil_test_access_timings_noplanes )

#ADD_EXECUTABLE( vil_test_pair_access_timings vil_test_pair_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_pair_access_timings vil mbl vcl )
#ADD_TEST( vil_test_pair_access_timings vil_test_pair_access_timings )

#ADD_EXECUTABLE( vil_test_random_access_timings vil_test_random_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_random_access_timings vil mbl vcl )
#ADD_TEST( vil_test_random_access_timings vil_test_random_access_timings )

ADD_EXECUTABLE( vil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_test_include vil )
