# DESCRIPTION:
#
#   This is the vipl_with_section library, containing the (templated)
#   image processing algorithm implementations for vxl, specialised for
#   the class "section" (instead of vil_image) as 2D pixel container.
#

USES += vxl tbl

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# Build this library only if not using implicit template instantiation
ifneq (1,$(USE_IMPLICIT_TEMPLATES))
LIBRARY := vipl_with_section
VERSION := 1.0
endif

MANPAGE_SOURCES += accessors/vipl_accessors_section.txx

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# compile with -O2 for efficiency:
debug = $(optimize)

