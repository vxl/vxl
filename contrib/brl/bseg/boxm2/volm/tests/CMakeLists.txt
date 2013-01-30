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
  test_volm_obj_based_matcher.cxx
  test_volm_matcher_p1.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_volm_test_all testlib boxm2_volm )

ADD_TEST( boxm2_volm_test_wr3db_ind_vrml        ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_test_all  test_volm_wr3db_ind  )
ADD_TEST( boxm2_volm_test_locations             ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_test_all  test_volm_locations  )
IF(OPENCL_FOUND)
ADD_TEST( boxm2_volm_obj_based_matcher          ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_test_all  test_volm_obj_based_matcher  )
ADD_TEST( boxm2_volm_matcher_p1                 ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_test_all  test_volm_matcher_p1  )
ENDIF(OPENCL_FOUND)

ADD_EXECUTABLE( boxm2_volm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_test_include boxm2_volm)
