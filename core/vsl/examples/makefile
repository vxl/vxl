#######################################
include ${IUEROOT}/config/usr_params.mk
#######################################

USES += vxl
include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vsl/client.mk

# Harris corner detector
MINI_PROG_SOURCES += run_vsl_harris.cxx

#
MINI_PROG_SOURCES += run_vsl_canny.cxx
MINI_PROG_SOURCES += topology_edges.cxx
MINI_PROG_SOURCES += break_lines.cxx

ald_libs := vsl vbl vil

######################################
include ${IUEROOT}/config/usr_rules.mk
######################################
