# mul/clsfy/tests/CMakeLists.txt

LINK_LIBRARIES( vul vpl vnl mbl clsfy testlib vpdfl pdf1d vsl)

ADD_EXECUTABLE (clsfy_test_binary_hyperplane     test_binary_hyperplane.cxx)
ADD_EXECUTABLE (clsfy_test_binary_pdf_classifier test_binary_pdf_classifier.cxx)
ADD_EXECUTABLE (clsfy_test_k_nearest_neighbour   test_k_nearest_neighbour.cxx)
ADD_EXECUTABLE (clsfy_test_smo_1                 test_smo_1.cxx)
ADD_EXECUTABLE (clsfy_test_rbf_svm_smo           test_rbf_svm_smo.cxx)
ADD_EXECUTABLE (clsfy_test_adaboost              test_adaboost.cxx)
ADD_EXECUTABLE (clsfy_test_direct_boost          test_direct_boost.cxx)
ADD_EXECUTABLE (clsfy_test_binary_threshold_1d   test_binary_threshold_1d.cxx)
ADD_EXECUTABLE (clsfy_test_mean_square_1d        test_mean_square_1d.cxx)

ADD_TEST (clsfy_test_binary_hyperplane     ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_binary_hyperplane)
ADD_TEST (clsfy_test_binary_pdf_classifier ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_binary_pdf_classifier)
ADD_TEST (clsfy_test_k_nearest_neighbour   ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_k_nearest_neighbour)
ADD_TEST (clsfy_test_smo_1                 ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_smo_1)
ADD_TEST (clsfy_test_rbf_svm_smo           ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_rbf_svm_smo)
ADD_TEST (clsfy_test_adaboost              ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_adaboost)
ADD_TEST (clsfy_test_direct_boost          ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_direct_boost)
ADD_TEST (clsfy_test_binary_threshold_1d   ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_binary_threshold_1d)
ADD_TEST (clsfy_test_mean_square_1d        ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_mean_square_1d)

ADD_EXECUTABLE( clsfy_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( clsfy_test_include clsfy )
