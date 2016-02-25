# mul/clsfy/tests/CMakeLists.txt

link_libraries(${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vnl mbl clsfy ${VXL_LIB_PREFIX}testlib vpdfl pdf1d vimt ${VXL_LIB_PREFIX}vsl)
add_executable(clsfy_test_all
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

add_test( NAME clsfy_test_binary_hyperplane COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_hyperplane)
add_test( NAME clsfy_test_binary_pdf_classifier COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_pdf_classifier)
add_test( NAME clsfy_test_k_nearest_neighbour COMMAND $<TARGET_FILE:clsfy_test_all> test_k_nearest_neighbour)
add_test( NAME clsfy_test_smo_1 COMMAND $<TARGET_FILE:clsfy_test_all> test_smo_1)
add_test( NAME clsfy_test_rbf_svm_smo COMMAND $<TARGET_FILE:clsfy_test_all> test_rbf_svm_smo)
add_test( NAME clsfy_test_adaboost COMMAND $<TARGET_FILE:clsfy_test_all> test_adaboost)
add_test( NAME clsfy_test_direct_boost COMMAND $<TARGET_FILE:clsfy_test_all> test_direct_boost)
add_test( NAME clsfy_test_binary_threshold_1d COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_threshold_1d)
add_test( NAME clsfy_test_mean_square_1d COMMAND $<TARGET_FILE:clsfy_test_all> test_mean_square_1d)
add_test( NAME clsfy_test_logit_loss_function COMMAND $<TARGET_FILE:clsfy_test_all> test_logit_loss_function)
add_test( NAME clsfy_test_binary_hyperplane_logit COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_hyperplane_logit)
add_test( NAME clsfy_test_binary_1d_wrapper COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_1d_wrapper)
add_test( NAME clsfy_test_binary_tree COMMAND $<TARGET_FILE:clsfy_test_all> test_binary_tree)
add_test( NAME clsfy_test_random_forest COMMAND $<TARGET_FILE:clsfy_test_all> test_random_forest)

add_executable( clsfy_test_include test_include.cxx )
target_link_libraries( clsfy_test_include clsfy )
