USES += contrib/brl contrib/brl/bbas core

LIBRARY := brdb
VERSION := 1.0

SOURCES += brdb_database.cxx
MANPAGE_SOURCES += brdb_database_sptr.h
SOURCES += brdb_database_manager.cxx
SOURCES += brdb_query.cxx
MANPAGE_SOURCES += brdb_query_aptr.h
SOURCES += brdb_relation.cxx
MANPAGE_SOURCES += brdb_relation_sptr.h
SOURCES += brdb_selection.cxx
MANPAGE_SOURCES += brdb_selection_sptr.h
SOURCES += brdb_tuple.cxx
MANPAGE_SOURCES += brdb_tuple_sptr.h
SOURCES += brdb_value.cxx
MANPAGE_SOURCES += brdb_value.txx
MANPAGE_SOURCES += brdb_value_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
