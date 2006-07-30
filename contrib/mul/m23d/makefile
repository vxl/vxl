USES += contrib/mul core

LIBRARY := m23d
VERSION := 1.0

SOURCES += m23d_correction_matrix_error.cxx
SOURCES += m23d_make_ortho_projection.cxx
SOURCES += m23d_ortho_flexible_builder.cxx
SOURCES += m23d_ortho_rigid_builder.cxx
SOURCES += m23d_pure_ortho_projection.cxx
SOURCES += m23d_rotation_from_ortho_projection.cxx
SOURCES += m23d_rotation_matrix.cxx
SOURCES += m23d_scaled_ortho_projection.cxx
SOURCES += m23d_select_basis_views.cxx
SOURCES += m23d_set_q_constraint.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
