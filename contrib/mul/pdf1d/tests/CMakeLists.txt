#
# ./mul/pdf1d/tests/CMakeLists.txt
#
#   Tim Cootes
#


INCLUDE( ${allvxl_SOURCE_DIR}/mul/pdf1d/CMakeListsLink.txt )

ADD_EXECUTABLE(
 test_pdf1d test_pdf1d.cxx
)

ADD_TEST(pdf1d_test_pdf1d test_pdf1d)
