# vxl/vil1/examples/CMakeLists.txt

LINK_LIBRARIES( vil1 vul )

ADD_EXECUTABLE(vil1_convolve_1d   vil1_convolve_1d.cxx)
ADD_EXECUTABLE(vil1_convolve      vil1_convolve.cxx)
ADD_EXECUTABLE(vil1_convert       vil1_convert.cxx)
ADD_EXECUTABLE(vil1_crop          vil1_crop.cxx)
ADD_EXECUTABLE(vil1_info          vil1_info.cxx)
ADD_EXECUTABLE(vil1_print_section vil1_print_section.cxx)
ADD_EXECUTABLE(vil1_scale         vil1_scale.cxx)
ADD_EXECUTABLE(vil1_flipud        vil1_flipud.cxx)
ADD_EXECUTABLE(vil1_clamp_example vil1_clamp_example.cxx)
ADD_EXECUTABLE(vil1_scale_intensities_example vil1_scale_intensities_example.cxx)
ADD_EXECUTABLE(vil1_rgb_example   vil1_rgb_example.cxx)
