# vxl/vil/examples/CMakeLists.txt

LINK_LIBRARIES( vil vul )

ADD_EXECUTABLE(vil_convolve_1d   vil_convolve_1d.cxx)
ADD_EXECUTABLE(vil_convolve      vil_convolve.cxx)
ADD_EXECUTABLE(vil_convert       vil_convert.cxx)
ADD_EXECUTABLE(vil_crop          vil_crop.cxx)
ADD_EXECUTABLE(vil_info          vil_info.cxx)
ADD_EXECUTABLE(vil_print_section vil_print_section.cxx)
ADD_EXECUTABLE(vil_scale         vil_scale.cxx)
ADD_EXECUTABLE(vil_flipud        vil_flipud.cxx)
ADD_EXECUTABLE(vil_clamp_example vil_clamp_example.cxx)
ADD_EXECUTABLE(vil_scale_intensities_example vil_scale_intensities_example.cxx)
ADD_EXECUTABLE(vil_rgb_example   vil_rgb_example.cxx)
