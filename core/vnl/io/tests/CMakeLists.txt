# ./vxl/vnl/io/tests/CMakeLists.txt

LINK_LIBRARIES( vnl_io testlib )

ADD_EXECUTABLE( vnl_io_test_all vnl_io_test_all.cxx)
ADD_EXECUTABLE( test_vnl_io test_vnl_io.cxx)
ADD_TEST( vnl_test_vnl_io test_vnl_io)
