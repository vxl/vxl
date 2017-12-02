# mul/vimt/examples/CMakeLists.txt

link_libraries( vimt_algo vimt ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul )

add_executable( vimt_example1 example1.cxx )
add_executable( vimt_example2 example2.cxx )
add_executable( vimt_example3 example3.cxx )
add_executable( vimt_example4 example4.cxx )
add_executable( make_dog_pyramid make_dog_pyramid.cxx )
add_executable( vimt_rotate_image rotate_image.cxx )
