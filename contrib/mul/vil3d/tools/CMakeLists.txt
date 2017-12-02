# mul/vil3d/tools/CMakeLists.txt
#
#  Tim Cootes
#
link_libraries(vil3d ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul vil3d_algo)

add_executable(vil3d_slice_image vil3d_slice_image.cxx)
add_executable(vil3d_byte_image_histo vil3d_byte_image_histo.cxx)
