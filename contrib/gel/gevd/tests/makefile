USES := vxl gel

include $(IUEROOT)/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vil/config.mk

IULIBS := -lgevd -lvdgl -lvtol -lvsol -lvgl-algo -lvgl -lvnl-algo -lvnl -lvil -lvbl -lvul

TESTS += test_gevd_bufferxy.cxx
TESTS += test_gevd_memory_mixin.cxx
TESTS += test_gevd_param_mixin.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

