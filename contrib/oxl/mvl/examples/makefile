USES = v3p vxl oxl

## vil_stream_url uses tcp sockets
USE_SOCKETS := 1

IULIBS += -lmvl -lvgl -lvil -lvnl-algo -lnetlib -lvnl -lvbl -lvpl -lvcl

MINI_PROG_SOURCES += mvl_multi_view_matches_example.cxx
MINI_PROG_SOURCES += $(wildcard six_point_structure.cxx)

include ${IUEROOT}/vxl/vil/vil_config.mk

################################
include ${IUEROOT}/config/top.mk
################################
