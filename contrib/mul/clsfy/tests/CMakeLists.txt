#
# ./mul/clsfy/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/mul/clsfy/CMakeListsLink.txt )

ADD_EXECUTABLE (test_binary_hyperplane test_binary_hyperplane.cxx)
ADD_EXECUTABLE (test_binary_pdf_classifier test_binary_pdf_classifier.cxx)
ADD_EXECUTABLE (test_k_nearest_neighbour test_k_nearest_neighbour.cxx)


ADD_TEST (test_binary_hyperplane  test_binary_hyperplane)
ADD_TEST (test_binary_pdf_classifier test_binary_pdf_classifier)
ADD_TEST (test_k_nearest_neighbour test_k_nearest_neighbour)
