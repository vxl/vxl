LINK_LIBRARIES( vdgl )

ADD_EXECUTABLE(test_vdgl test_vdgl.cxx )
ADD_TEST(test_vdgl test_vdgl)
ADD_EXECUTABLE( test1_vdgl test1_vdgl.cxx )
ADD_TEST(test1_vdgl test1_vdgl)

ADD_EXECUTABLE( vdgl_test_include test_include.cxx )
