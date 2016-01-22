ADD_EXECUTABLE( boxm2_vecf_test_all
  test_driver.cxx
  test_eye.cxx
  test_orbit.cxx
  test_fit_orbit.cxx
  test_fit_margin.cxx
  test_pc_viewer.cxx
  test_mandible.cxx
  test_cranium.cxx
 )
TARGET_LINK_LIBRARIES(boxm2_vecf_test_all testlib  boxm2_vecf brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl )

##- #depends on OPENCL being found...
##- INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
##- IF(OPENCL_FOUND)

ADD_TEST( NAME boxm2_vecf_test_eye COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_eye  )
ADD_TEST( NAME boxm2_vecf_test_orbit COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_orbit  )
ADD_TEST( NAME boxm2_vecf_test_fit_orbit COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_fit_orbit  )
ADD_TEST( NAME boxm2_vecf_test_fit_margin COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_fit_margin  )
ADD_TEST( NAME boxm2_vecf_test_pc_viewer COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_pc_viewer  )
ADD_TEST( NAME boxm2_vecf_test_mandible COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_mandible  )
ADD_TEST( NAME boxm2_vecf_test_cranium COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_cranium  )

#ADD_EXECUTABLE( boxm2_vecf_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_vecf_test_include boxm2_vecf )

##- ENDIF(OPENCL_FOUND)
