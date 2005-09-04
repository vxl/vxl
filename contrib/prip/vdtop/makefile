USES += contrib/prip core

LIBRARY := vdtop
VERSION := 1.0

SOURCES += vdtop_8_neighborhood_mask.cxx
SOURCES += vdtop_freeman_code.cxx
MANPAGE_SOURCES += vdtop_4_lower_homotopic_kernel.h
MANPAGE_SOURCES += vdtop_4_lower_leveling_kernel.h
MANPAGE_SOURCES += vdtop_4_upper_homotopic_kernel.h
MANPAGE_SOURCES += vdtop_4_upper_leveling_kernel.h
MANPAGE_SOURCES += vdtop_8_lower_homotopic_kernel.h
MANPAGE_SOURCES += vdtop_8_lower_leveling_kernel.h
MANPAGE_SOURCES += vdtop_8_upper_homotopic_kernel.h
MANPAGE_SOURCES += vdtop_8_upper_leveling_kernel.h
MANPAGE_SOURCES += vdtop_construct_well_composed.h
MANPAGE_SOURCES += vdtop_draw_digital_graph.h
MANPAGE_SOURCES += vdtop_kernel.h
MANPAGE_SOURCES += vdtop_misc.txx
MANPAGE_SOURCES += vdtop_neighborhood.h
MANPAGE_SOURCES += vdtop_pixel.h
MANPAGE_SOURCES += vdtop_pixel.txx
MANPAGE_SOURCES += vdtop_replace_quasi_8_minima.h
MANPAGE_SOURCES += vdtop_set_4_veinerization_structure.h
MANPAGE_SOURCES += vdtop_set_4_veinerization_structure.txx
MANPAGE_SOURCES += vdtop_set_8_veinerization_structure.h
MANPAGE_SOURCES += vdtop_set_8_veinerization_structure.txx
MANPAGE_SOURCES += vdtop_set_structure_from_digital_graph.h
MANPAGE_SOURCES += vdtop_set_structure_from_digital_graph.txx
MANPAGE_SOURCES += vdtop_well_composed_lower_leveling_kernel.h
MANPAGE_SOURCES += vil_canny_deriche_grad_filter.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
