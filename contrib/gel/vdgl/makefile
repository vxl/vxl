USES += contrib/gel core

IMMEDIATE_LIBS := -lvsol -lvgl -lvbl -lvnl -lvul

LIBRARY := vdgl
VERSION := 1.0

# background structure
SOURCES += vdgl_edgel.cxx
SOURCES += vdgl_edgel_chain.cxx
SOURCES += vdgl_interpolator.cxx

# interpolators
SOURCES += vdgl_interpolator_linear.cxx
SOURCES += vdgl_interpolator_cubic.cxx

# and outside interface
SOURCES += vdgl_digital_curve.cxx
SOURCES += vdgl_digital_region.cxx

SOURCES += vdgl_fit_line.cxx
SOURCES += vdgl_fit_lines.cxx
SOURCES += vdgl_fit_lines_params.cxx
SOURCES += vdgl_ortho_regress.cxx
MANPAGE_SOURCES += vdgl_OrthogRegress.h

MANPAGE_SOURCES += vdgl_digital_curve_sptr.h
MANPAGE_SOURCES += vdgl_edgel_chain_sptr.h
MANPAGE_SOURCES += vdgl_interpolator_sptr.h
MANPAGE_SOURCES += vdgl_digital_region_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
