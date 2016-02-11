# This is core/vil/tests/CMakeLists.txt

add_executable( vil_test_all
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

target_link_libraries( vil_test_all ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl )

# basic things
add_test( NAME vil_test_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_image_resource)
add_test( NAME vil_test_image_view COMMAND $<TARGET_FILE:vil_test_all> test_image_view)
add_test( NAME vil_test_memory_chunk COMMAND $<TARGET_FILE:vil_test_all> test_memory_chunk)
add_test( NAME vil_test_pixel_format COMMAND $<TARGET_FILE:vil_test_all> test_pixel_format)
add_test( NAME vil_test_border COMMAND $<TARGET_FILE:vil_test_all> test_border)
add_test( NAME vil_test_round COMMAND $<TARGET_FILE:vil_test_all> test_round)
add_test( NAME vil_test_pyramid_image_view COMMAND $<TARGET_FILE:vil_test_all> test_pyramid_image_view)

# file format readers/writers
add_test( NAME vil_test_file_format_read COMMAND $<TARGET_FILE:vil_test_all> test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( NAME vil_test_save_load_image COMMAND $<TARGET_FILE:vil_test_all> test_save_load_image)
add_test( NAME vil_test_image_loader_robustness COMMAND $<TARGET_FILE:vil_test_all> test_image_loader_robustness)
add_test( NAME vil_test_stream COMMAND $<TARGET_FILE:vil_test_all> test_stream ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( NAME vil_test_4_plane_tiff COMMAND $<TARGET_FILE:vil_test_all> test_4_plane_tiff ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
# image operations
add_test( NAME vil_test_deep_copy_3_plane COMMAND $<TARGET_FILE:vil_test_all> test_deep_copy_3_plane )
add_test( NAME vil_test_image_view_maths COMMAND $<TARGET_FILE:vil_test_all> test_image_view_maths)
add_test( NAME vil_test_convert COMMAND $<TARGET_FILE:vil_test_all> test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( NAME vil_test_rotate_image COMMAND $<TARGET_FILE:vil_test_all> test_rotate_image)
add_test( NAME vil_test_warp COMMAND $<TARGET_FILE:vil_test_all> test_warp)

# Sampling Operations
add_test( NAME vil_test_bilin_interp COMMAND $<TARGET_FILE:vil_test_all> test_bilin_interp)
add_test( NAME vil_test_sample_profile_bilin COMMAND $<TARGET_FILE:vil_test_all> test_sample_profile_bilin)
add_test( NAME vil_test_sample_grid_bilin COMMAND $<TARGET_FILE:vil_test_all> test_sample_grid_bilin)
add_test( NAME vil_test_resample_bilin COMMAND $<TARGET_FILE:vil_test_all> test_resample_bilin)
add_test( NAME vil_test_bicub_interp COMMAND $<TARGET_FILE:vil_test_all> test_bicub_interp)
add_test( NAME vil_test_sample_profile_bicub COMMAND $<TARGET_FILE:vil_test_all> test_sample_profile_bicub)
add_test( NAME vil_test_sample_grid_bicub COMMAND $<TARGET_FILE:vil_test_all> test_sample_grid_bicub)
add_test( NAME vil_test_resample_bicub COMMAND $<TARGET_FILE:vil_test_all> test_resample_bicub)
add_test( NAME vil_test_nearest_interp COMMAND $<TARGET_FILE:vil_test_all> test_nearest_interp)
add_test( NAME vil_test_resample_nearest COMMAND $<TARGET_FILE:vil_test_all> test_resample_nearest)

# Math
add_test( NAME vil_test_math_value_range COMMAND $<TARGET_FILE:vil_test_all> test_math_value_range)
add_test( NAME vil_test_math_median COMMAND $<TARGET_FILE:vil_test_all> test_math_median)
add_test( NAME vil_test_na COMMAND $<TARGET_FILE:vil_test_all> test_na)

# Blocked images
add_test( NAME vil_test_blocked_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_blocked_image_resource ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

# Pyramid images
add_test( NAME vil_test_image_list COMMAND $<TARGET_FILE:vil_test_all> test_image_list )
add_test( NAME vil_test_pyramid_image_resource COMMAND $<TARGET_FILE:vil_test_all> test_pyramid_image_resource ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

# To run timing tests
#add_executable( vil_test_access_timings vil_test_access_timings.cxx)
#target_link_libraries( vil_test_access_timings ${VXL_LIB_PREFIX}vil mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_test_access_timings COMMAND $<TARGET_FILE:vil_test_access_timings> )

#add_executable( vil_test_access_timings_noplanes vil_test_access_timings_noplanes.cxx)
#target_link_libraries( vil_test_access_timings_noplanes ${VXL_LIB_PREFIX}vil mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_test_access_timings_noplanes COMMAND $<TARGET_FILE:vil_test_access_timings_noplanes> )

#add_executable( vil_test_pair_access_timings vil_test_pair_access_timings.cxx)
#target_link_libraries( vil_test_pair_access_timings ${VXL_LIB_PREFIX}vil mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_test_pair_access_timings COMMAND $<TARGET_FILE:vil_test_pair_access_timings> )

#add_executable( vil_test_random_access_timings vil_test_random_access_timings.cxx)
#target_link_libraries( vil_test_random_access_timings ${VXL_LIB_PREFIX}vil mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_test_random_access_timings COMMAND $<TARGET_FILE:vil_test_random_access_timings> )

add_executable( vil_test_include test_include.cxx )
target_link_libraries( vil_test_include ${VXL_LIB_PREFIX}vil )
add_executable( vil_test_template_include test_template_include.cxx )
target_link_libraries( vil_test_template_include ${VXL_LIB_PREFIX}vil )
