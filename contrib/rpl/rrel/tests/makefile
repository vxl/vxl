USES += contrib/rpl contrib/mul core
IULIBS += -lrrel_tests
include ../config.mk
include $(IUEROOT)/core/testlib/config.mk

LIBRARY := rrel_tests

TESTMAIN := test_driver.cxx
SOURCES += similarity_from_matches.cxx
TESTS += test_homography2d_est.cxx
TESTS += test_irls.cxx
TESTS += test_linear_reg.cxx
TESTS += test_lms_lts.cxx
TESTS += test_m_est_obj.cxx
TESTS += test_orthogonal_regression.cxx
TESTS += test_ran_sam_search.cxx
TESTS += test_ransac_obj.cxx
TESTS += test_robust_util.cxx
TESTS += test_similarity_from_matches.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
