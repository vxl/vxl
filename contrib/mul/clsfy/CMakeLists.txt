# mul/clsfy/CMakeLists.txt

SET(clsfy_sources
 clsfy_binary_hyperplane.cxx                  clsfy_binary_hyperplane.h
 clsfy_binary_hyperplane_ls_builder.cxx       clsfy_binary_hyperplane_ls_builder.h
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
)

AUX_SOURCE_DIRECTORY(Templates clsfy_sources)

ADD_LIBRARY(clsfy ${clsfy_sources})
INSTALL_TARGETS(/lib clsfy)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/mul/clsfy ${clsfy_sources})
TARGET_LINK_LIBRARIES(clsfy vpdfl mbl vnl_algo vnl_io vnl vbl_io vbl vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
