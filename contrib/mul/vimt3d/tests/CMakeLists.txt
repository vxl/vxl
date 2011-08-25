# This is mul/vimt3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt3d_test_all
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

TARGET_LINK_LIBRARIES( vimt3d_test_all vimt3d vil3d mbl vgl vnl vil vpl vul vsl testlib )

ADD_TEST( vimt3d_test_transform_3d                ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_transform_3d )
ADD_TEST( vimt3d_test_image_3d_of                 ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_image_3d_of )
ADD_TEST( vimt3d_test_load                        ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_load ${mul_SOURCE_DIR}/vil3d/tests/file_read_data)
ADD_TEST( vimt3d_test_save                        ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_save)
ADD_TEST( vimt3d_test_sample_grid_trilin          ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_sample_grid_trilin )
ADD_TEST( vimt3d_test_v3i                         ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_v3i )
ADD_TEST( vimt3d_test_v3m                         ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_v3m )
ADD_TEST( vimt3d_test_from_image_2d               ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_from_image_2d )
ADD_TEST( vimt3d_test_gauss_reduce                ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_gauss_reduce )
ADD_TEST( vimt3d_test_resample                    ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_resample )
ADD_TEST( vimt3d_test_image_bounds_and_centre_3d  ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_image_bounds_and_centre_3d )
ADD_TEST( vimt3d_test_reflect                     ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_reflect )
ADD_TEST( vimt3d_test_crop                        ${EXECUTABLE_OUTPUT_PATH}/vimt3d_test_all test_crop )


ADD_EXECUTABLE( vimt3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vimt3d_test_include vimt3d )
ADD_EXECUTABLE( vimt3d_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vimt3d_test_template_include vimt3d )
