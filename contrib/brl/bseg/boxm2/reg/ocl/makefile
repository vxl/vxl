USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_reg_ocl

SOURCES += boxm2_ocl_reg_mutual_info.cxx
SOURCES += boxm2_ocl_reg_minfo_func.cxx
SOURCES += boxm2_ocl_reg_depth_map_to_vol.cxx

SUBDIRS += exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
