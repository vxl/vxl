USES += contrib/brl contrib/gel contrib/brl/bbas core
include ../config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/contrib/gel/vtol/config.mk
include ${IUEROOT}/core/vnl/config.mk

ifdef HAS_XERCES

TESTS += bxml_test_map.cxx
TESTS += bxml_test_vdgl_digital_curve_input_converter.cxx
TESTS += bxml_test_vsol_point_2d_input_converter.cxx
TESTS += bxml_test_vtol_vertex_2d_input_converter.cxx
TESTS += bxml_test_vtol_zero_chain_2d_input_converter.cxx
TESTS += bxml_test_vtol_edge_2d_input_converter.cxx
TESTS += bxml_test_vector_vtol_edge_2d_input_conv.cxx

endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
