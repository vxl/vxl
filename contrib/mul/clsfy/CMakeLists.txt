# ./mul/clsfy/CMakeLists.txt

ADD_LIBRARY(clsfy clsfy_sources)

SOURCE_FILES(clsfy_sources
  clsfy_binary_hyperplane.cxx
  clsfy_binary_hyperplane_ls_builder.cxx
  clsfy_binary_pdf_classifier.cxx
  clsfy_builder_base.cxx
  clsfy_classifier_base.cxx
  clsfy_simple_adaboost.cxx
  clsfy_k_nearest_neighbour.cxx
  clsfy_rbf_parzen.cxx
  clsfy_knn_builder.cxx
  clsfy_parzen_builder.cxx
  clsfy_random_builder.cxx
  clsfy_random_classifier.cxx
  clsfy_classifier_1d.cxx
  clsfy_binary_threshold_1d.cxx
  clsfy_builder_1d.cxx
)

AUX_SOURCE_DIRECTORY(Templates clsfy_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/mul/clsfy/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
