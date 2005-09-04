ifneq (,$(wildcard ${VXLROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
endif

# additions to vnl
SUBDIRS += bnl

# spatial object algorithms
SUBDIRS += bsol

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

# staged vgl development area
SUBDIRS += bvgl

# uncertainty geometry library
SUBDIRS += bugl

# Brown Graph Library
SUBDIRS += bgrl

# Brown statistics library
SUBDIRS += bsta

# Brown dynamic programming library
SUBDIRS += bdpg

################################
include ${VXLROOT}/config/top.mk
################################
