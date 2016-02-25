# This brl/bbas/bugl/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR} )

add_executable( bugl_test_all
  test_driver.cxx
  test_bugl.cxx
  test_random_transform_set_2d.cxx
)
target_link_libraries( bugl_test_all bugl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )

add_test( NAME bugl_test_bugl COMMAND $<TARGET_FILE:bugl_test_all> test_bugl)
add_test( NAME bugl_test_random_transform_2d COMMAND $<TARGET_FILE:bugl_test_all> test_random_transform_set_2d)

add_executable( bugl_test_include test_include.cxx)
target_link_libraries( bugl_test_include bugl)
add_executable( bugl_test_template_include test_template_include.cxx)
target_link_libraries( bugl_test_template_include bugl)
