# some tests depend on OPENCL
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
ENDIF(OPENCL_FOUND)

ADD_EXECUTABLE( boxm2_volm_test_all
  test_driver.cxx
  test_volm_wr3db_ind.cxx
  test_volm_locations.cxx
  test_volm_matcher_p1.cxx
  test_volm_matcher_p0.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_volm_test_all testlib boxm2_volm )

ADD_TEST( NAME boxm2_volm_test_wr3db_ind_vrml COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_wr3db_ind  )
ADD_TEST( NAME boxm2_volm_test_locations      COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_locations  )
ADD_TEST( NAME boxm2_volm_matcher_p0          COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_matcher_p0 )
ADD_TEST( NAME boxm2_volm_matcher_p1          COMMAND $<TARGET_FILE:boxm2_volm_test_all>  test_volm_matcher_p1 )

ADD_EXECUTABLE( boxm2_volm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_test_include boxm2_volm)
