# vxl/vil1/examples/CMakeLists.txt

link_libraries( ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vul )

add_executable(vil1_convolve_1d   vil1_convolve_1d.cxx)
add_executable(vil1_convolve      vil1_convolve.cxx)
add_executable(vil1_convert       vil1_convert.cxx)
add_executable(vil1_crop          vil1_crop.cxx)
add_executable(vil1_info          vil1_info.cxx)
add_executable(vil1_print_section vil1_print_section.cxx)
add_executable(vil1_scale         vil1_scale.cxx)
add_executable(vil1_flipud        vil1_flipud.cxx)
add_executable(vil1_clamp_example vil1_clamp_example.cxx)
add_executable(vil1_scale_intensities_example vil1_scale_intensities_example.cxx)
add_executable(vil1_rgb_example   vil1_rgb_example.cxx)
