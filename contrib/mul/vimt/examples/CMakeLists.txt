# mul/vimt/examples/CMakeLists.txt

LINK_LIBRARIES( vimt_algo vimt vnl vil vgl vul )

ADD_EXECUTABLE( vimt_example1 example1.cxx )
ADD_EXECUTABLE( vimt_example2 example2.cxx )
ADD_EXECUTABLE( vimt_example3 example3.cxx )
ADD_EXECUTABLE( vimt_example4 example4.cxx )
ADD_EXECUTABLE( make_dog_pyramid make_dog_pyramid.cxx )
ADD_EXECUTABLE( vimt_rotate_image rotate_image.cxx )
