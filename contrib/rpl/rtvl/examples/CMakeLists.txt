add_executable(rtvl_hello rtvl_hello.cxx)
target_link_libraries(rtvl_hello rtvl ${VXL_LIB_PREFIX}vnl)

add_executable(rtvl_saliency_image rtvl_saliency_image.cxx)
target_link_libraries(rtvl_saliency_image rtvl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil)
