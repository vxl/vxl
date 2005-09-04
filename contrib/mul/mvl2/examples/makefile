USES += contrib/mul core
include ../config.mk

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifeq ($(HAS_AVI),1)
  MINI_PROG_SOURCES += copy_image.cxx
  MINI_PROG_SOURCES += mvl2_to_divx.cxx
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
