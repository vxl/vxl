USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boct
VERSION := 1.0

SOURCES += boct_tree_cell_base.cxx
MANPAGE_SOURCES += boct_tree_cell_base_sptr.h
SOURCES += boct_loc_code.cxx
MANPAGE_SOURCES += boct_test_util.txx
MANPAGE_SOURCES += boct_tree.txx
MANPAGE_SOURCES += boct_tree_cell.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
