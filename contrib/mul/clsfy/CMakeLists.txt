#
# ./mul/clsfy/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(clsfy clsfy_sources)

SOURCE_FILES(clsfy_sources
clsfy_binary_hyperplane.cxx
clsfy_binary_hyperplane_ls_builder.cxx
clsfy_binary_pdf_classifier.cxx
clsfy_builder_base.cxx
clsfy_classifier_base.cxx
clsfy_k_nearest_neighbour.cxx
clsfy_rbf_parzen.cxx
clsfy_knn_builder.cxx
clsfy_parzen_builder.cxx
clsfy_random_builder.cxx
clsfy_random_classifier.cxx
)


IF (WIN32)
  IF (NOT CYGWIN)
    SOURCE_FILES(clsfy_sources
    clsfy_binary_hyperplane.h
    clsfy_binary_hyperplane_ls_builder.h
    clsfy_binary_pdf_classifier.h
    clsfy_builder_base.h
    clsfy_classifier_base.h
    clsfy_k_nearest_neighbour.h
    clsfy_rbf_parzen.h
    clsfy_knn_builder.h
    clsfy_parzen_builder.h
    clsfy_random_builder.h
    clsfy_random_classifier.h
    )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)


AUX_SOURCE_DIRECTORY(Templates clsfy_sources)


IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)