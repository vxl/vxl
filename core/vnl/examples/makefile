##########################################
include ${IUEROOT}/config/usr_params.mk
##########################################
USES += vxl

include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
ald_libs := vbl vnl-algo

ifneq ($(NOTEST), 1)
#MINI_PROG_SOURCES += time_fastops.cxx
#MINI_PROG_SOURCES += vnl-calc.cxx
endif

##########################################
include ${IUEROOT}/config/usr_rules.mk
##########################################
