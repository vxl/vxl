#######################################
include ${IUEROOT}/config/top-params.mk
#######################################
USES += vxl
IULIBS += -lvsl -lvil -lvnl-algo -lvnl -lvbl -lvcl

# Harris corner detector
MINI_PROG_SOURCES += run_vsl_harris.cxx

#
MINI_PROG_SOURCES += run_vsl_canny.cxx
MINI_PROG_SOURCES += topology_edges.cxx
MINI_PROG_SOURCES += break_lines.cxx

ald_libs := vsl vbl #vil is deduced from vsl.

include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vsl/client.mk
######################################
include ${IUEROOT}/config/top-rules.mk
######################################
