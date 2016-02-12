add_executable( boxm2_vecf_test_all
  test_driver.cxx
  test_eye.cxx
  test_orbit.cxx
  test_fit_orbit.cxx
  test_fit_margin.cxx
  test_pc_viewer.cxx
  test_mandible.cxx
  test_cranium.cxx
 )
target_link_libraries(boxm2_vecf_test_all ${VXL_LIB_PREFIX}testlib  boxm2_vecf brdb ${VXL_LIB_PREFIX}vpgl_algo vpgl_pro vil_pro sdet ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl )

##- #depends on OPENCL being found...
##- include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
##- if(OPENCL_FOUND)

add_test( NAME boxm2_vecf_test_eye COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_eye  )
add_test( NAME boxm2_vecf_test_orbit COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_orbit  )
add_test( NAME boxm2_vecf_test_fit_orbit COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_fit_orbit  )
add_test( NAME boxm2_vecf_test_fit_margin COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_fit_margin  )
add_test( NAME boxm2_vecf_test_pc_viewer COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_pc_viewer  )
add_test( NAME boxm2_vecf_test_mandible COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_mandible  )
add_test( NAME boxm2_vecf_test_cranium COMMAND $<TARGET_FILE:boxm2_vecf_test_all>  test_cranium  )

#add_executable( boxm2_vecf_test_include test_include.cxx )
#target_link_libraries( boxm2_vecf_test_include boxm2_vecf )

##- endif()
