USES += contrib/brl contrib/gel contrib/brl/bbas core
ifneq (,$(wildcard ${IUEROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
 INCDIRS += ${IUEROOT}/v3p/xerces-c-src1_4_0/include
endif

ifdef HAS_XERCES

LIBRARY := bxml
VERSION := 1.0

SOURCES += bxml_io.cxx
SOURCES += bxml_vdgl_digital_curve_input_converter.cxx
SOURCES += bxml_vsol_point_2d_input_converter.cxx
SOURCES += bxml_vtol_edge_2d_input_converter.cxx
SOURCES += bxml_vtol_io.cxx
SOURCES += bxml_vtol_vertex_2d_input_converter.cxx
SOURCES += bxml_vtol_zero_chain_2d_input_converter.cxx
SOURCES += bxml_input_converter.cxx
MANPAGE_SOURCES += bxml_generic_ptr.h
MANPAGE_SOURCES += bxml_input_converter_sptr.h
MANPAGE_SOURCES += bxml_vsol_point_2d_input_converter_sptr.h

endif

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

