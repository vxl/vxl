MANPAGE_SOURCES += vxl_copyright.h
MANPAGE_SOURCES += vxl_version.h
MANPAGE_SOURCES += vxl_config.h.in

# common test executable
SUBDIRS += testlib

# POSIX layer
SUBDIRS += vpl

# utilities
SUBDIRS += vul

# "basics" (templates)
SUBDIRS += vbl

# numerics
SUBDIRS += vnl

# geometry (including computational geometry)
SUBDIRS += vgl

# image
SUBDIRS += vil vil1 vil/algo

# stream I/O
SUBDIRS += vsl vul/io vbl/io vnl/io vgl/io vil/io vil1/io

# coordinate systems
SUBDIRS += vcsl

# video
SUBDIRS += vidl

# video (using vil1)
SUBDIRS += vidl_vil1

# multi-platform GUI toolkit
SUBDIRS += vgui

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# global examples and tests
ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
