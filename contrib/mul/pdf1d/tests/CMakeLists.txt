# mul/pdf1d/tests/CMakeLists.txt

TARGET_LINK_LIBRARIES( test_pdf1d pdf1d testlib )

ADD_EXECUTABLE( test_pdf1d test_pdf1d.cxx )
ADD_TEST( pdf1d_test_pdf1d test_pdf1d     )
