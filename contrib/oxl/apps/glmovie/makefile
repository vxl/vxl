USES = vxl oxl v3p
USE_OPENGL := 1
USE_GLUT := 1
TRY_MPEG := 1

include ${IUEROOT}/vxl/vidl/config.mk
include ${IUEROOT}/oxl/oxp/config.mk
include $(IUEROOT)/vxl/vpl/config.mk

MINI_PROG_SOURCES += vobgetframe.cxx

PROGRAM = glmovie
SOURCES += glmovie.cxx
SOURCES += make_index.cxx

################################
include $(IUEROOT)/config/top.mk
################################

