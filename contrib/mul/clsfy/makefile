USES := vxl mul

LIBRARY := clsfy
VERSION := 1.0

SOURCES += clsfy_binary_hyperplane.cxx
SOURCES += clsfy_binary_hyperplane_ls_builder.cxx
SOURCES += clsfy_binary_pdf_classifier.cxx
SOURCES += clsfy_builder_base.cxx
SOURCES += clsfy_classifier_base.cxx
SOURCES += clsfy_k_nearest_neighbour.cxx
SOURCES += clsfy_knn_builder.cxx
SOURCES += clsfy_parzen_builder.cxx
SOURCES += clsfy_random_builder.cxx
SOURCES += clsfy_random_classifier.cxx
SOURCES += clsfy_rbf_parzen.cxx
SOURCES += clsfy_binary_threshold_1d.cxx
SOURCES += clsfy_builder_1d.cxx
SOURCES += clsfy_classifier_1d.cxx
SOURCES += clsfy_simple_adaboost.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
