USES = vxl rpl mul
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk

TESTS += test_borgefors.cxx
TESTS += test_rsdl_bins_2d.cxx
TESTS += test_rsdl_bounding_box.cxx
TESTS += test_rsdl_dist.cxx
TESTS += test_rsdl_kd_tree.cxx
TESTS += test_rsdl_point.cxx

IULIBS := -lrsdl
IULIBS += -lmbl -lvbl -lvnl-io -lvnl-algo -lvgl-io -lvgl -lvnl -lvbl -lvsl -lvul

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
