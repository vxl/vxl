IF(OPENCL_FOUND)
  ADD_EXECUTABLE( boxm2_volm_pro_test_include test_include.cxx )
  TARGET_LINK_LIBRARIES( boxm2_volm_pro_test_include boxm2_volm_pro )
ENDIF(OPENCL_FOUND)
