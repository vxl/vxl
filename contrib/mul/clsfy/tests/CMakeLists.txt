#
# ./mul/clsfy/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/clsfy/CMakeListsLink.txt )

ADD_EXECUTABLE (clsfy_test_binary_hyperplane test_binary_hyperplane.cxx)
ADD_EXECUTABLE (clsfy_test_binary_pdf_classifier test_binary_pdf_classifier.cxx)
ADD_EXECUTABLE (clsfy_test_k_nearest_neighbour test_k_nearest_neighbour.cxx)
ADD_EXECUTABLE (clsfy_test_smo_1 test_smo_1.cxx)
ADD_EXECUTABLE (clsfy_test_rbf_svm_smo test_rbf_svm_smo.cxx)
ADD_EXECUTABLE (clsfy_test_adaboost test_adaboost.cxx)
ADD_EXECUTABLE (clsfy_test_binary_threshold_1d test_binary_threshold_1d.cxx)


ADD_TEST (clsfy_test_binary_hyperplane  clsfy_test_binary_hyperplane)
ADD_TEST (clsfy_test_binary_pdf_classifier clsfy_test_binary_pdf_classifier)
ADD_TEST (clsfy_test_k_nearest_neighbour clsfy_test_k_nearest_neighbour)
ADD_TEST (clsfy_test_smo_1 clsfy_test_smo_1)
ADD_TEST (clsfy_test_rbf_svm_smo clsfy_test_rbf_svm_smo)
ADD_TEST (clsfy_test_adaboost test_adaboost)
ADD_TEST (clsfy_test_binary_threshold_1d test_binary_threshold_1d)
