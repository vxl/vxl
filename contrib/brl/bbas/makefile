ifneq (,$(wildcard ${IUEROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
endif

# topology algorithms
SUBDIRS += btol

# digital library algorithms
SUBDIRS += bdgl

# xml<->vxl conversion 
ifdef HAS_XERCES
 SUBDIRS += bxml
endif

# basic vgui classes
SUBDIRS += bgui

################################
include ${IUEROOT}/config/top.mk
################################
