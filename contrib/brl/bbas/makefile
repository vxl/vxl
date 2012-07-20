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

# Brown graph library
SUBDIRS += bgrl

# Brown templated graph library
SUBDIRS += bgrl2

# Brown statistics library
SUBDIRS += bsta

# Brown dynamic programming library
SUBDIRS += bdpg

# 3D mesh library
SUBDIRS += bmsh3d

# 3D modeling library
SUBDIRS += bwm

# in-memory relational database
SUBDIRS += brdb

# Brown imaging library
SUBDIRS += bil

# indexed mesh library
SUBDIRS += imesh

# vrml utilities library
SUBDIRS += bvrml

# Brown opencl library
SUBDIRS += bocl

# Brown asychronous IO library
SUBDIRS += baio

# Brown radiometry library
SUBDIRS += brad

# viewsphere Library
SUBDIRS += vsph

# camera library
SUBDIRS += bpgl

# Brown hadoop library
SUBDIRS += bhdfs

# Brown subjective logic library
SUBDIRS += bsl

# Brown elastic curve matching library
SUBDIRS += bcvr

################################
include ${VXLROOT}/config/top.mk
################################
