USES := vxl gel
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
SOURCES += vdgl_ortho_regress.cxx
SOURCES += vdgl_OrthogRegress.cxx

MANPAGE_SOURCES += vdgl_digital_curve_sptr.h
MANPAGE_SOURCES += vdgl_edgel_chain_sptr.h
MANPAGE_SOURCES += vdgl_interpolator_sptr.h
MANPAGE_SOURCES += vdgl_digital_region_sptr.h

# These two are compiled in vtol:
MANPAGE_SOURCES += vdgl_intensity_face.h
MANPAGE_SOURCES += vdgl_intensity_face_sptr.h

ifndef NOTEST
SUBDIRS += tests examples
endif

################################
include ${IUEROOT}/config/top.mk
################################
