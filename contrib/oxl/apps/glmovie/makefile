USES += contrib/oxl core
USE_OPENGL := 1
USE_GLUT := 1
TRY_MPEG2 := 1
include ${VXLROOT}/core/vidl_vil1/config.mk
include ${VXLROOT}/contrib/oxl/oxp/config.mk
include $(VXLROOT)/core/vpl/config.mk

MINI_PROG_SOURCES += vobgetframe.cxx

PROGRAM = glmovie
SOURCES += glmovie.cxx
SOURCES += make_index.cxx

################################
include $(VXLROOT)/config/top.mk
################################
