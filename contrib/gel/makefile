#
# crossge@crd.ge.com
#

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# basics
SUBDIRS += gbl

# van-duc region finder (ported from TargetJr)
SUBDIRS += gevd

# matcher
SUBDIRS += geml

# Multi-view database
SUBDIRS += gmvl

# Dense matcher
SUBDIRS += vsrl

# Triangulation code
SUBDIRS += gtrl

# spatial objects
SUBDIRS += vsol

# topology
SUBDIRS += gst vtol

# digital geometry
SUBDIRS += vdgl

# photogrammetry
SUBDIRS += mrc/vpgl

# coordinate systems addon
SUBDIRS += mrc/vcsl

# general examples
ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

