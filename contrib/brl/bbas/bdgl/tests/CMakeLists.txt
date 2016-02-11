# brl/bbas/bdgl/tests/CMakeLists.txt

include_directories(${GEL_INCLUDE_DIR} )

add_executable( bdgl_test_all
  test_driver.cxx
  test_curve_algs.cxx
  test_region_algs.cxx
  test_peano_curve.cxx
)

target_link_libraries( bdgl_test_all bdgl vdgl vsol ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_test( NAME bdgl_test_curve_algs COMMAND $<TARGET_FILE:bdgl_test_all> test_curve_algs)
add_test( NAME bdgl_test_region_algs COMMAND $<TARGET_FILE:bdgl_test_all> test_region_algs)
add_test( NAME bdgl_test_peano_curve COMMAND $<TARGET_FILE:bdgl_test_all> test_peano_curve)

add_executable( bdgl_test_include test_include.cxx )
target_link_libraries( bdgl_test_include bdgl )
