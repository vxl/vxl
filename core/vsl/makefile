#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := vsl
VERSION := 1.0


SOURCES += vsl_indent.cxx
SOURCES += vsl_test.cxx
SOURCES += vsl_binary_io.cxx
SOURCES += vsl_binary_loader_base.cxx

# Additional files to document:
# MANPAGE_SOURCES += yyyyyyyyyyy.txx

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
