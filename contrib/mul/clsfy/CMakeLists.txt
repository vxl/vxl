# mul/clsfy/CMakeLists.txt

doxygen_add_library(contrib/mul/clsfy
  DEPENDS contrib/mul/vpdfl contrib/mul/mbl core/vnl core/vbl
  PACKAGE contrib/mul
  DESCRIPTION "Various statistical classifiers"
  )

set(clsfy_sources
 clsfy_add_all_loaders.cxx                    clsfy_add_all_loaders.h
 clsfy_binary_hyperplane.cxx                  clsfy_binary_hyperplane.h
 clsfy_binary_hyperplane_ls_builder.cxx       clsfy_binary_hyperplane_ls_builder.h
 clsfy_binary_hyperplane_gmrho_builder.cxx    clsfy_binary_hyperplane_gmrho_builder.h
 clsfy_binary_hyperplane_logit_builder.cxx    clsfy_binary_hyperplane_logit_builder.h
 clsfy_binary_pdf_classifier.cxx              clsfy_binary_pdf_classifier.h
 clsfy_builder_base.cxx                       clsfy_builder_base.h
 clsfy_classifier_base.cxx                    clsfy_classifier_base.h
 clsfy_simple_adaboost.cxx                    clsfy_simple_adaboost.h
 clsfy_k_nearest_neighbour.cxx                clsfy_k_nearest_neighbour.h
 clsfy_rbf_parzen.cxx                         clsfy_rbf_parzen.h
 clsfy_knn_builder.cxx                        clsfy_knn_builder.h
 clsfy_parzen_builder.cxx                     clsfy_parzen_builder.h
 clsfy_random_builder.cxx                     clsfy_random_builder.h
 clsfy_random_classifier.cxx                  clsfy_random_classifier.h
 clsfy_null_classifier.cxx                    clsfy_null_classifier.h
 clsfy_null_builder.cxx                       clsfy_null_builder.h
 clsfy_classifier_1d.cxx                      clsfy_classifier_1d.h
 clsfy_binary_threshold_1d.cxx                clsfy_binary_threshold_1d.h
 clsfy_binary_threshold_1d_builder.cxx        clsfy_binary_threshold_1d_builder.h
 clsfy_builder_1d.cxx                         clsfy_builder_1d.h
 clsfy_smo_base.cxx                           clsfy_smo_base.h
 clsfy_smo_1.cxx                              clsfy_smo_1.h
 clsfy_rbf_svm.cxx                            clsfy_rbf_svm.h
 clsfy_rbf_svm_smo_1_builder.cxx              clsfy_rbf_svm_smo_1_builder.h
 clsfy_adaboost_trainer.cxx                   clsfy_adaboost_trainer.h

 clsfy_adaboost_sorted_builder.cxx            clsfy_adaboost_sorted_builder.h
 clsfy_mean_square_1d.cxx                     clsfy_mean_square_1d.h
 clsfy_mean_square_1d_builder.cxx             clsfy_mean_square_1d_builder.h
 clsfy_direct_boost.cxx                       clsfy_direct_boost.h
 clsfy_direct_boost_builder.cxx               clsfy_direct_boost_builder.h
 clsfy_binary_1d_wrapper.cxx                  clsfy_binary_1d_wrapper.h
 clsfy_binary_1d_wrapper_builder.cxx          clsfy_binary_1d_wrapper_builder.h

 clsfy_logit_loss_function.cxx               clsfy_logit_loss_function.h
 clsfy_binary_threshold_1d_gini_builder.cxx        clsfy_binary_threshold_1d_gini_builder.h
 clsfy_binary_tree.cxx                clsfy_binary_tree.h
 clsfy_binary_tree_builder.cxx                clsfy_binary_tree_builder.h
 clsfy_random_forest.cxx                clsfy_random_forest.h
 clsfy_random_forest_builder.cxx                clsfy_random_forest_builder.h
)

aux_source_directory(Templates clsfy_sources)

vxl_add_library(LIBRARY_NAME clsfy LIBRARY_SOURCES ${clsfy_sources})
target_link_libraries(clsfy vpdfl mbl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
