USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_reg

SOURCES += boxm2_reg_mutual_info.cxx

SUBDIRS += ocl exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
