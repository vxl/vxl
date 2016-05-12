# some tests depend on OPENCL
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
endif()

add_executable( boxm2_volm_test_all
  test_driver.cxx
  test_volm_wr3db_ind.cxx
  test_volm_locations.cxx
  test_volm_matcher_p1.cxx
  test_volm_matcher_p0.cxx
 )

target_link_libraries( boxm2_volm_test_all ${VXL_LIB_PREFIX}testlib boxm2_volm )

add_test( NAME boxm2_volm_test_wr3db_ind_vrml COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_wr3db_ind  )
add_test( NAME boxm2_volm_test_locations      COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_locations  )
add_test( NAME boxm2_volm_matcher_p0          COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_matcher_p0 )
add_test( NAME boxm2_volm_matcher_p1          COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_matcher_p1 )

add_executable( boxm2_volm_test_include test_include.cxx )
target_link_libraries( boxm2_volm_test_include boxm2_volm)
