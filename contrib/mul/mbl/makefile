#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := mbl
VERSION := 1.0


SOURCES += mbl_gamma.cxx
SOURCES += mbl_mz_random.cxx
SOURCES += mbl_matxvec.cxx

# Additional files to document:
# MANPAGE_SOURCES += yyyyyyyyyyy.txx

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
