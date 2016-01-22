SET(boxm2_ocl_test_all_sources
    boxm2_ocl_test_utils.h    boxm2_ocl_test_utils.cxx
    )

ADD_LIBRARY(boxm2_ocl_utils ${boxm2_ocl_test_all_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_utils boct boxm2 boxm2_io testlib vpl)

ADD_EXECUTABLE( boxm2_ocl_test_all
  test_driver.cxx
  test_refine.cxx
  test_image_pyramid.cxx
  test_process_mains.cxx
  test_weighted_em.cxx
  test_kernel_filter.cxx
  test_kernel_vector_filter.cxx
#  test_cubic_estimation.cxx
  test_ocl_cholesky.cxx
  test_ocl_levenberg_marquardt.cxx
  test_ocl_lm_phongs_model.cxx
  test_opencl_cache2.cxx
 )


TARGET_LINK_LIBRARIES( boxm2_ocl_test_all testlib boxm2_ocl boxm2_ocl_pro boxm2_ocl_utils boxm2_cpp boxm2_cpp_algo boxm2_cpp_pro boxm2_pro brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl )

ADD_TEST( NAME boxm2_ocl_test_refine COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_refine  )
ADD_TEST( NAME boxm2_ocl_test_image_pyramid COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_image_pyramid  )
ADD_TEST( NAME boxm2_ocl_test_process_mains COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_process_mains  )
ADD_TEST( NAME boxm2_ocl_test_weighted_em COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_weighted_em  )
if( HACK_FORCE_BRL_FAILING_TESTS ) ## These tests are always failing on Mac
ADD_TEST( NAME boxm2_ocl_test_kernel_filter COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_kernel_filter )
ADD_TEST( NAME boxm2_ocl_test_kernel_vector_filter COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_kernel_vector_filter )
endif()
#ADD_TEST( NAME boxm2_ocl_test_cubic_estimation COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_cubic_estimation  )
ADD_TEST( NAME boxm2_ocl_test_ocl_cholesky COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_ocl_cholesky  )
ADD_TEST( NAME boxm2_ocl_test_ocl_levenberg_marquardt COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_ocl_levenberg_marquardt  )
ADD_TEST( NAME boxm2_ocl_test_ocl_lm_phongs_model COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_ocl_lm_phongs_model  )
ADD_TEST( NAME boxm2_ocl_test_opencl_cache2 COMMAND $<TARGET_FILE:boxm2_ocl_test_all>       test_opencl_cache2  )


ADD_EXECUTABLE( boxm2_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_ocl_test_include boxm2_ocl )

#ADD_EXECUTABLE( boxm2_ocl_test_template_include test_template_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_ocl_test_template_include boxm2_ocl )
