# mul/pdf1d/tests/CMakeLists.txt

LINK_LIBRARIES( pdf1d vpl testlib )
ADD_EXECUTABLE(pdf1d_test_all test_pdf1d.cxx )
ADD_TEST(pdf1d_test_pdf1d pdf1d_test_all)

ADD_EXECUTABLE( pdf1d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( pdf1d_test_include pdf1d )
