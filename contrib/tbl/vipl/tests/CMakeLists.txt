# ./tbl/vipl/tests/CMakeLists.txt

add_definitions(-DINSTANTIATE_TEMPLATES)

add_executable( vipl_test_all
  test_driver.cxx        test_driver.h

  vipl_test_threshold.cxx
  vipl_test_erode_disk.cxx
  vipl_test_gaussian_convolution.cxx
  vipl_test_histogram.cxx   # uses vnl_vector as container; remove if VXL_BUILD_CORE_NUMERICS is OFF
)
target_link_libraries( vipl_test_all vipl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib )

add_test( NAME vipl_test_threshold COMMAND $<TARGET_FILE:vipl_test_all> vipl_test_threshold)
add_test( NAME vipl_test_erode_disk COMMAND $<TARGET_FILE:vipl_test_all> vipl_test_erode_disk)
add_test( NAME vipl_test_gaussian_convolution COMMAND $<TARGET_FILE:vipl_test_all> vipl_test_gaussian_convolution)

if(VXL_BUILD_CORE_NUMERICS)
  target_link_libraries( vipl_test_all ${VXL_LIB_PREFIX}vnl )
  add_test( NAME vipl_test_histogram COMMAND $<TARGET_FILE:vipl_test_all> vipl_test_histogram)
endif()

add_executable( vipl_test_include test_include.cxx )
target_link_libraries( vipl_test_include vipl )
add_executable( vipl_test_template_include test_template_include.cxx )
target_link_libraries( vipl_test_template_include vipl )
