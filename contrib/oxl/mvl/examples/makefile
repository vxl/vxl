USES = v3p vxl oxl
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vnl/algo/config.mk

IULIBS += -lmvl -lvgl -lvil -lvnl-algo -lvnl -lvbl -lvpl -lvul

MINI_PROG_SOURCES += mvl_multi_view_matches_example.cxx
MINI_PROG_SOURCES += $(wildcard six_point_structure.cxx)

################################
include ${IUEROOT}/config/top.mk
################################
