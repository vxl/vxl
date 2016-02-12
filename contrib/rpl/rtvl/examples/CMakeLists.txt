add_executable(rtvl_hello rtvl_hello.cxx)
target_link_libraries(rtvl_hello rtvl ${VXL_LIB_PREFIX}vnl)
