#
# ./mul/pdf1d/tests/CMakeLists.txt
#
#   Tim Cootes
#


ADD_EXECUTABLE(
 test_pdf1d test_pdf1d.cxx
)
TARGET_LINK_LIBRARIES( test_pdf1d pdf1d )

ADD_TEST(pdf1d_test_pdf1d test_pdf1d)
