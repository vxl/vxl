if(OPENCL_FOUND)
  add_executable( boxm2_volm_pro_test_include test_include.cxx )
  target_link_libraries( boxm2_volm_pro_test_include boxm2_volm_pro )
endif()
