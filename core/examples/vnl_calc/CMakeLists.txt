add_executable( vnl_calc vnl_calc.cxx )
target_link_libraries( vnl_calc ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vcl )
