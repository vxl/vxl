VXL := 1 

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

# Harris corner detector
MINI_PROG_SOURCES += run_vsl_harris.cxx

IULIBS = -lvsl -lvil -lvbl -lvcl

include $(IUELOCALROOT)/vxl/vil/vil_config.mk

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
