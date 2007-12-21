ifneq (,$(wildcard ${VXLROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
endif

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

# coin3D incorporated vgui classes
SUBDIRS += bgui3d

# staged vgl development area
SUBDIRS += bvgl

# uncertainty geometry library
SUBDIRS += bugl

# additions to vnl
SUBDIRS += bnl

# Brown Graph Library
SUBDIRS += bgrl

# Brown statistics library
SUBDIRS += bsta

# Brown dynamic programming library
SUBDIRS += bdpg

#3D mesh library 
SUBDIRS += bmsh3d

#3D modeling library
SUBDIRS += bwm

# Video Library (rewrite)
SUBDIRS += vidl2

################################
include ${VXLROOT}/config/top.mk
################################
