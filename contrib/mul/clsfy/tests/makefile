USES += contrib/mul core
include ../config.mk
include ${VXLROOT}/contrib/mul/pdf1d/config.mk
include $(VXLROOT)/core/vul/config.mk
include $(VXLROOT)/core/vpl/config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTS += test_binary_hyperplane.cxx
TESTS += test_binary_pdf_classifier.cxx
TESTS += test_k_nearest_neighbour.cxx
TESTS += test_smo_1.cxx
TESTS += test_rbf_svm_smo.cxx
TESTS += test_adaboost.cxx
TESTS += test_binary_threshold_1d.cxx
TESTS += test_direct_boost.cxx
TESTS += test_mean_square_1d.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
