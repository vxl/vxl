# mul/pdf1d/tests/CMakeLists.txt

LINK_LIBRARIES( pdf1d vpl testlib )

ADD_EXECUTABLE( test_pdf1d test_pdf1d.cxx )
ADD_TEST( pdf1d_test_pdf1d test_pdf1d     )
