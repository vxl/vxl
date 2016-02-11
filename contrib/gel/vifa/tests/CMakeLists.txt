# gel/vifa/tests/CMakeLists.txt

include_directories(${GEL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR})

add_executable( vifa_test_all
  test_ifa_factory.h
  test_ifa_factory.cxx
  test_driver.cxx
  test_coll_lines_params.cxx
  test_group_pgram_params.cxx
  test_norm_params.cxx
  test_parallel_params.cxx
  test_int_face_attr_common_params.cxx
  test_image_histogram.cxx
  test_region_proc.cxx
  test_int_face_attr.cxx
  test_int_faces_attr.cxx
  test_int_faces_adj_attr.cxx
)
target_link_libraries( vifa_test_all vifa sdet vtol vdgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vifa_test_coll_lines_params COMMAND $<TARGET_FILE:vifa_test_all> test_coll_lines_params)
add_test( NAME vifa_test_group_pgram_params COMMAND $<TARGET_FILE:vifa_test_all> test_group_pgram_params)
add_test( NAME vifa_test_norm_params COMMAND $<TARGET_FILE:vifa_test_all> test_norm_params)
add_test( NAME vifa_test_parallel_params COMMAND $<TARGET_FILE:vifa_test_all> test_parallel_params)
add_test( NAME vifa_test_int_face_attr_common_params COMMAND $<TARGET_FILE:vifa_test_all> test_int_face_attr_common_params)
add_test( NAME vifa_test_image_histogram COMMAND $<TARGET_FILE:vifa_test_all> test_image_histogram ${CMAKE_CURRENT_SOURCE_DIR}/cam0013.tif)
add_test( NAME vifa_test_region_proc COMMAND $<TARGET_FILE:vifa_test_all> test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/testimg.tif)
add_test( NAME vifa_test_int_face_attr COMMAND $<TARGET_FILE:vifa_test_all> test_int_face_attr ${CMAKE_CURRENT_SOURCE_DIR}/testimg.tif)
add_test( NAME vifa_test_int_faces_attr COMMAND $<TARGET_FILE:vifa_test_all> test_int_faces_attr ${CMAKE_CURRENT_SOURCE_DIR}/testimg.tif)
add_test( NAME vifa_test_int_faces_adj_attr COMMAND $<TARGET_FILE:vifa_test_all> test_int_faces_adj_attr ${CMAKE_CURRENT_SOURCE_DIR}/testimg.tif)

add_executable( vifa_test_include test_include.cxx )
target_link_libraries( vifa_test_include vifa )
add_executable( vifa_test_template_include test_template_include.cxx )
target_link_libraries( vifa_test_template_include vifa )
