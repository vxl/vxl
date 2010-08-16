USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boct
VERSION := 1.0

MANPAGE_SOURCES += boct_loc_code.txx
MANPAGE_SOURCES += boct_test_util.txx
MANPAGE_SOURCES += boct_tree.txx
MANPAGE_SOURCES += boct_tree_cell.h
MANPAGE_SOURCES += boct_tree_cell.txx
MANPAGE_SOURCES += boct_tree_cell_reader.txx
SOURCES += boct_tree_cell.cxx
SOURCES += boct_bit_tree.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
