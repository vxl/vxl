# mul/vimt/examples/CMakeLists.txt

LINK_LIBRARIES( vimt vil_algo vil vul testlib)

ADD_EXECUTABLE( vimt_example1 example1.cxx )
ADD_EXECUTABLE( vimt_example2 example2.cxx )
ADD_EXECUTABLE( vimt_example3 example3.cxx )
ADD_EXECUTABLE( vimt_example4 example4.cxx )
ADD_EXECUTABLE( make_dog_pyramid make_dog_pyramid.cxx )

