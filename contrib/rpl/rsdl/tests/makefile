USES = vxl rpl mul
include ${IUEROOT}/vxl/vnl/algo/config.mk

TESTS += test_borgefors.cxx
TESTS += test_rsdl_bins_2d.cxx
TESTS += test_rsdl_bounding_box.cxx
TESTS += test_rsdl_dist.cxx
TESTS += test_rsdl_kd_tree.cxx
TESTS += test_rsdl_point.cxx

IULIBS := -lrsdl -lvnl -lvbl
IULIBS += -lmbl -lvbl -lvnl-algo -lvsl -lvnl-io -lvul

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
