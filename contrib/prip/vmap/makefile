USES += contrib/prip core

LIBRARY := vmap
VERSION := 1.0

SOURCES += vbl_controlled_partition.cxx
SOURCES += vmap_2_map.cxx
SOURCES += vmap_2_tmap_elts.cxx
SOURCES += vmap_grid_structure.cxx
MANPAGE_SOURCES += vmap_2_map.txx
MANPAGE_SOURCES += vmap_2_tmap.txx
MANPAGE_SOURCES += vmap_kernel.txx
MANPAGE_SOURCES += vmap_map_iterator_wrapper.h
MANPAGE_SOURCES += vmap_non_oriented_kernel.txx
MANPAGE_SOURCES += vmap_oriented_kernel.txx
MANPAGE_SOURCES += vmap_ptr_sequences.h
MANPAGE_SOURCES += vmap_set_grid_structure.h
MANPAGE_SOURCES += vmap_types.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
