USES = vxl gel
include $(IUEROOT)/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk

TESTS += test_vertex_2d.cxx
TESTS += test_zero_chain.cxx
TESTS += test_edge_2d.cxx
TESTS += test_one_chain.cxx
TESTS += test_face_2d.cxx
TESTS += test_two_chain.cxx
TESTS += test_block.cxx

IULIBS += -lvtol -lvsol -lvdgl -lvgl-algo -lvgl -lvnl-algo -lvnl -lvbl -lvul

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
