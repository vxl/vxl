##########################################
include ${IUEROOT}/config/top-params.mk
##########################################

USES = vxl

LIBRARY := vbl-test-templates

SOURCES += vbl_dummy.cxx

IULIBS := -lvcl -lvbl
VXL := 1

##########################################
include ${IUEROOT}/config/top-rules.mk
##########################################
