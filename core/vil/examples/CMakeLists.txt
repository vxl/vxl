# core/vil/examples/CMakeLists.txt

LINK_LIBRARIES( vil vcl )

ADD_EXECUTABLE( vil_example1 example1.cxx )
ADD_EXECUTABLE( vil_example2 example2.cxx )
ADD_EXECUTABLE( vil_example3 example3.cxx )
ADD_EXECUTABLE( vil_example4 example4.cxx )
ADD_EXECUTABLE( vil_example5 example5.cxx )
ADD_EXECUTABLE( vil_example6 example6.cxx )
ADD_EXECUTABLE( vil_file_example1 file_example1.cxx )
ADD_EXECUTABLE( vil_file_example2 file_example2.cxx )
ADD_EXECUTABLE( vil_file_example3 file_example3.cxx )
ADD_EXECUTABLE( vil_file_example4 file_example4.cxx )
ADD_EXECUTABLE( vil_image_copy vil_image_copy.cxx )
ADD_EXECUTABLE( vil_rotate_image vil_rotate_image.cxx )
ADD_EXECUTABLE( vil_resample_image vil_resample_image.cxx )
ADD_EXECUTABLE( vil_example_interpolate example_interpolate.cxx )
ADD_EXECUTABLE( vil_generate_nitf_pyramid_images generate_nitf_pyramid_images.cxx )
