# This is mul/vimt3d/tests/CMakeLists.txt

add_executable( vimt3d_test_all
  test_driver.cxx
  test_transform_3d.cxx
  test_image_3d_of.cxx
  test_load.cxx
  test_save.cxx
  test_sample_grid_trilin.cxx
  test_v3i.cxx
  test_v3m.cxx
  test_from_image_2d.cxx
  test_resample.cxx
  test_gauss_reduce.cxx
  test_image_bounds_and_centre_3d.cxx
  test_reflect.cxx
  test_crop.cxx
)

target_link_libraries( vimt3d_test_all vimt3d vil3d mbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vimt3d_test_transform_3d COMMAND $<TARGET_FILE:vimt3d_test_all> test_transform_3d )
add_test( NAME vimt3d_test_image_3d_of COMMAND $<TARGET_FILE:vimt3d_test_all> test_image_3d_of )
add_test( NAME vimt3d_test_load COMMAND $<TARGET_FILE:vimt3d_test_all> test_load ${mul_SOURCE_DIR}/vil3d/tests/file_read_data)
add_test( NAME vimt3d_test_save COMMAND $<TARGET_FILE:vimt3d_test_all> test_save)
add_test( NAME vimt3d_test_sample_grid_trilin COMMAND $<TARGET_FILE:vimt3d_test_all> test_sample_grid_trilin )
add_test( NAME vimt3d_test_v3i COMMAND $<TARGET_FILE:vimt3d_test_all> test_v3i )
add_test( NAME vimt3d_test_v3m COMMAND $<TARGET_FILE:vimt3d_test_all> test_v3m )
add_test( NAME vimt3d_test_from_image_2d COMMAND $<TARGET_FILE:vimt3d_test_all> test_from_image_2d )
add_test( NAME vimt3d_test_gauss_reduce COMMAND $<TARGET_FILE:vimt3d_test_all> test_gauss_reduce )
add_test( NAME vimt3d_test_resample COMMAND $<TARGET_FILE:vimt3d_test_all> test_resample )
add_test( NAME vimt3d_test_image_bounds_and_centre_3d COMMAND $<TARGET_FILE:vimt3d_test_all> test_image_bounds_and_centre_3d )
add_test( NAME vimt3d_test_reflect COMMAND $<TARGET_FILE:vimt3d_test_all> test_reflect )
add_test( NAME vimt3d_test_crop COMMAND $<TARGET_FILE:vimt3d_test_all> test_crop )


add_executable( vimt3d_test_include test_include.cxx )
target_link_libraries( vimt3d_test_include vimt3d )
add_executable( vimt3d_test_template_include test_template_include.cxx )
target_link_libraries( vimt3d_test_template_include vimt3d )
