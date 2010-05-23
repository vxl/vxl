# mul/clsfy/tests/CMakeLists.txt

LINK_LIBRARIES(vgl vul vpl vnl mbl clsfy testlib vpdfl pdf1d vimt vsl)
ADD_EXECUTABLE (clsfy_test_all
  test_driver.cxx

  test_binary_hyperplane.cxx
  test_binary_pdf_classifier.cxx
  test_k_nearest_neighbour.cxx
  test_smo_1.cxx
  test_rbf_svm_smo.cxx
  test_adaboost.cxx
  test_direct_boost.cxx
  test_binary_threshold_1d.cxx
  test_mean_square_1d.cxx
  test_logit_loss_function.cxx
  test_binary_hyperplane_logit.cxx
  test_binary_1d_wrapper.cxx
  test_binary_tree.cxx
  test_random_forest.cxx
)

ADD_TEST (clsfy_test_binary_hyperplane        ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_hyperplane)
ADD_TEST (clsfy_test_binary_pdf_classifier    ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_pdf_classifier)
ADD_TEST (clsfy_test_k_nearest_neighbour      ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_k_nearest_neighbour)
ADD_TEST (clsfy_test_smo_1                    ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_smo_1)
ADD_TEST (clsfy_test_rbf_svm_smo              ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_rbf_svm_smo)
ADD_TEST (clsfy_test_adaboost                 ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_adaboost)
ADD_TEST (clsfy_test_direct_boost             ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_direct_boost)
ADD_TEST (clsfy_test_binary_threshold_1d      ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_threshold_1d)
ADD_TEST (clsfy_test_mean_square_1d           ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_mean_square_1d)
ADD_TEST (clsfy_test_logit_loss_function      ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_logit_loss_function)
ADD_TEST (clsfy_test_binary_hyperplane_logit  ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_hyperplane_logit)
ADD_TEST (clsfy_test_binary_1d_wrapper        ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_1d_wrapper)
ADD_TEST (clsfy_test_binary_tree              ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_binary_tree)
ADD_TEST (clsfy_test_random_forest            ${EXECUTABLE_OUTPUT_PATH}/clsfy_test_all test_random_forest)

ADD_EXECUTABLE( clsfy_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( clsfy_test_include clsfy )
