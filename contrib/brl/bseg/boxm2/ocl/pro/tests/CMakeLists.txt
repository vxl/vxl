add_executable( boxm2_ocl_pro_test_all
  test_driver.cxx
# not registered -- FIX_ME
#  test_batch_update.cxx
  test_filter_kernel.cxx
 )

target_link_libraries( boxm2_ocl_pro_test_all testlib boxm2_ocl_pro brdb boct vpgl_algo vpgl_pro vil_pro sdet vnl vul vpl )

#add_test( NAME boxm2_ocl_pro_test_batch_update COMMAND $<TARGET_FILE:boxm2_ocl_pro_test_all>  test_batch_update)
add_test( NAME boxm2_ocl_pro_test_filter_kernel COMMAND $<TARGET_FILE:boxm2_ocl_pro_test_all>  test_filter_kernel)

add_executable( boxm2_ocl_pro_test_include test_include.cxx )
target_link_libraries( boxm2_ocl_pro_test_include boxm2_ocl )
