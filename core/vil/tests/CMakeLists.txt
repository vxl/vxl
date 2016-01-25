# This is core/vil/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_test_all
  test_driver.cxx

  # basic things
  test_image_resource.cxx
  test_blocked_image_resource.cxx
  test_image_view.cxx
  test_memory_chunk.cxx
  test_pixel_format.cxx
  test_pyramid_image_resource.cxx
  test_border.cxx
  test_round.cxx
  test_pyramid_image_view.cxx

  # file format readers/writers
  test_file_format_read.cxx
  test_save_load_image.cxx
  test_image_loader_robustness.cxx
  test_stream.cxx
  test_image_list.cxx
  test_4_plane_tiff.cxx

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
  test_nearest_interp.cxx
  # test_sample_profile_nearest.cxx
  # test_sample_grid_nearest.cxx
  test_resample_nearest.cxx


  # Math
  test_math_value_range.cxx
  test_math_median.cxx
  test_na.cxx
)

TARGET_LINK_LIBRARIES( vil_test_all vil vpl vul testlib vcl )

# basic things
ADD_TEST( NAME vil_test_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_image_resource)
ADD_TEST( NAME vil_test_image_view COMMAND $<TARGET_FILE:vil_test_all> test_image_view)
ADD_TEST( NAME vil_test_memory_chunk COMMAND $<TARGET_FILE:vil_test_all> test_memory_chunk)
ADD_TEST( NAME vil_test_pixel_format COMMAND $<TARGET_FILE:vil_test_all> test_pixel_format)
ADD_TEST( NAME vil_test_border COMMAND $<TARGET_FILE:vil_test_all> test_border)
ADD_TEST( NAME vil_test_round COMMAND $<TARGET_FILE:vil_test_all> test_round)
ADD_TEST( NAME vil_test_pyramid_image_view COMMAND $<TARGET_FILE:vil_test_all> test_pyramid_image_view)

# file format readers/writers
ADD_TEST( NAME vil_test_file_format_read COMMAND $<TARGET_FILE:vil_test_all> test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( NAME vil_test_save_load_image COMMAND $<TARGET_FILE:vil_test_all> test_save_load_image)
ADD_TEST( NAME vil_test_image_loader_robustness COMMAND $<TARGET_FILE:vil_test_all> test_image_loader_robustness)
ADD_TEST( NAME vil_test_stream COMMAND $<TARGET_FILE:vil_test_all> test_stream ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( NAME vil_test_4_plane_tiff COMMAND $<TARGET_FILE:vil_test_all> test_4_plane_tiff ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
# image operations
ADD_TEST( NAME vil_test_deep_copy_3_plane COMMAND $<TARGET_FILE:vil_test_all> test_deep_copy_3_plane )
ADD_TEST( NAME vil_test_image_view_maths COMMAND $<TARGET_FILE:vil_test_all> test_image_view_maths)
ADD_TEST( NAME vil_test_convert COMMAND $<TARGET_FILE:vil_test_all> test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( NAME vil_test_rotate_image COMMAND $<TARGET_FILE:vil_test_all> test_rotate_image)
ADD_TEST( NAME vil_test_warp COMMAND $<TARGET_FILE:vil_test_all> test_warp)

# Sampling Operations
ADD_TEST( NAME vil_test_bilin_interp COMMAND $<TARGET_FILE:vil_test_all> test_bilin_interp)
ADD_TEST( NAME vil_test_sample_profile_bilin COMMAND $<TARGET_FILE:vil_test_all> test_sample_profile_bilin)
ADD_TEST( NAME vil_test_sample_grid_bilin COMMAND $<TARGET_FILE:vil_test_all> test_sample_grid_bilin)
ADD_TEST( NAME vil_test_resample_bilin COMMAND $<TARGET_FILE:vil_test_all> test_resample_bilin)
ADD_TEST( NAME vil_test_bicub_interp COMMAND $<TARGET_FILE:vil_test_all> test_bicub_interp)
ADD_TEST( NAME vil_test_sample_profile_bicub COMMAND $<TARGET_FILE:vil_test_all> test_sample_profile_bicub)
ADD_TEST( NAME vil_test_sample_grid_bicub COMMAND $<TARGET_FILE:vil_test_all> test_sample_grid_bicub)
ADD_TEST( NAME vil_test_resample_bicub COMMAND $<TARGET_FILE:vil_test_all> test_resample_bicub)
ADD_TEST( NAME vil_test_nearest_interp COMMAND $<TARGET_FILE:vil_test_all> test_nearest_interp)
ADD_TEST( NAME vil_test_resample_nearest COMMAND $<TARGET_FILE:vil_test_all> test_resample_nearest)

# Math
ADD_TEST( NAME vil_test_math_value_range COMMAND $<TARGET_FILE:vil_test_all> test_math_value_range)
ADD_TEST( NAME vil_test_math_median COMMAND $<TARGET_FILE:vil_test_all> test_math_median)
ADD_TEST( NAME vil_test_na COMMAND $<TARGET_FILE:vil_test_all> test_na)

# Blocked images
ADD_TEST( NAME vil_test_blocked_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_blocked_image_resource ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

# Pyramid images
ADD_TEST( NAME vil_test_image_list COMMAND $<TARGET_FILE:vil_test_all> test_image_list )
ADD_TEST( NAME vil_test_pyramid_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_pyramid_image_resource ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

# To run timing tests
#ADD_EXECUTABLE( vil_test_access_timings vil_test_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings vil mbl vcl )
#ADD_TEST( NAME vil_test_access_timings COMMAND $<TARGET_FILE:vil_test_access_timings> )

#ADD_EXECUTABLE( vil_test_access_timings_noplanes vil_test_access_timings_noplanes.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings_noplanes vil mbl vcl )
#ADD_TEST( NAME vil_test_access_timings_noplanes COMMAND $<TARGET_FILE:vil_test_access_timings_noplanes> )

#ADD_EXECUTABLE( vil_test_pair_access_timings vil_test_pair_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_pair_access_timings vil mbl vcl )
#ADD_TEST( NAME vil_test_pair_access_timings COMMAND $<TARGET_FILE:vil_test_pair_access_timings> )

#ADD_EXECUTABLE( vil_test_random_access_timings vil_test_random_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_random_access_timings vil mbl vcl )
#ADD_TEST( NAME vil_test_random_access_timings COMMAND $<TARGET_FILE:vil_test_random_access_timings> )

ADD_EXECUTABLE( vil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_test_include vil )
ADD_EXECUTABLE( vil_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vil_test_template_include vil )
