USES = vxl rpl mul
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk

LIBRARY := rrel_tests
SOURCES := similarity_from_matches.cxx

TESTMAIN := test_driver.cxx
TESTS += test_include.cxx
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

IULIBS += -lrrel_tests -lrrel -lmbl -lvbl -lvnl-io -lvnl-algo -lvnl -lvgl-io -lvsl -lvgl -lvul -ltestlib

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
