add_executable( boxm2_vecf_ocl_test_all
  test_driver.cxx
  test_transform.cxx
  test_store_nbrs.cxx
  test_filter.cxx
 )



target_link_libraries(boxm2_vecf_ocl_test_all ${VXL_LIB_PREFIX}testlib  boxm2_vecf boxm2_vecf_ocl brdb ${VXL_LIB_PREFIX}vpgl_algo vpgl_pro vil_pro sdet ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl )

add_test( NAME boxm2_vecf_ocl_test_transform COMMAND $<TARGET_FILE:boxm2_vecf_ocl_test_all>  test_transform  )
add_test( NAME boxm2_vecf_ocl_test_store_nbrs COMMAND $<TARGET_FILE:boxm2_vecf_ocl_test_all>  test_store_nbrs  )
add_test( NAME boxm2_vecf_ocl_test_filter COMMAND $<TARGET_FILE:boxm2_vecf_ocl_test_all>  test_filter  )


#add_executable( boxm2_ocl_test_include test_include.cxx )
#target_link_libraries( boxm2_ocl_test_include boxm2_ocl )

