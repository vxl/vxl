# DESCRIPTION:
#
#   This is the vipl_with_section library, containing the (templated)
#   image processing algorithm implementations for vxl, specialised for
#   the class "section" (instead of vil_image) as 2D pixel container.
#

USES += vxl tbl

LIBRARY := vipl_with_section
VERSION := 1.0

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

MANPAGE_SOURCES += accessors/vipl_accessors_section.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# No need to compile the files in Templates when implicitly instantiating:
ifeq ($(USE_IMPLICIT_TEMPLATES),1)
  TEMPLATE_SOURCES :=
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# compile with -O2 for efficiency:
debug = $(optimize)

