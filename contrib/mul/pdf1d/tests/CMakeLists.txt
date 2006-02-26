# mul/pdf1d/tests/CMakeLists.txt

ADD_EXECUTABLE(pdf1d_test_all test_pdf1d.cxx )
TARGET_LINK_LIBRARIES( pdf1d_test_all pdf1d mbl vnl vsl vpl testlib )
ADD_TEST(pdf1d_test_pdf1d pdf1d_test_all)

ADD_EXECUTABLE( pdf1d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( pdf1d_test_include pdf1d )
