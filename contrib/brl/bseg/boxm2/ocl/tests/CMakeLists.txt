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


SET(boxm2_ocl_test_all_sources
    boxm2_ocl_test_utils.h    boxm2_ocl_test_utils.cxx
    )

ADD_LIBRARY(boxm2_ocl_utils  ${boxm2_ocl_test_all_sources})

TARGET_LINK_LIBRARIES( boxm2_ocl_test_all testlib  boxm2_ocl boxm2_ocl_pro boxm2_ocl_utils boxm2_cpp boxm2_cpp_algo boxm2_cpp_pro boxm2_pro brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl )

ADD_TEST( boxm2_ocl_test_refine                      ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_refine  )
ADD_TEST( boxm2_ocl_test_image_pyramid               ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_image_pyramid  )
ADD_TEST( boxm2_ocl_test_process_mains               ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_process_mains  )
ADD_TEST( boxm2_ocl_test_weighted_em                 ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_weighted_em  )
ADD_TEST( boxm2_ocl_test_kernel_filter               ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_kernel_filter )
ADD_TEST( boxm2_ocl_test_kernel_vector_filter        ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_kernel_vector_filter )
#ADD_TEST( boxm2_ocl_test_cubic_estimation           ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_cubic_estimation  )
ADD_TEST( boxm2_ocl_test_ocl_cholesky                ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_ocl_cholesky  )
ADD_TEST( boxm2_ocl_test_ocl_levenberg_marquardt     ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_ocl_levenberg_marquardt  )
ADD_TEST( boxm2_ocl_test_ocl_lm_phongs_model     ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_ocl_lm_phongs_model  )
ADD_TEST( boxm2_ocl_test_opencl_cache2     ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all       test_opencl_cache2  )


ADD_EXECUTABLE( boxm2_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_ocl_test_include boxm2_ocl )

#ADD_EXECUTABLE( boxm2_ocl_test_template_include test_template_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_ocl_test_template_include boxm2_ocl )
