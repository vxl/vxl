##########################################
include ${IUEROOT}/config/usr_params.mk
##########################################

USES = vxl

LIBRARY := vbl-test-templates

#SOURCES += vbl_dummy.cxx

IULIBS := -lvcl -lvbl

##########################################
include ${IUEROOT}/config/usr_rules.mk
##########################################
