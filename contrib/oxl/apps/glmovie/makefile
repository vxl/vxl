USES += contrib/oxl core
USE_OPENGL := 1
USE_GLUT := 1
TRY_MPEG2 := 1
include ${IUEROOT}/core/vidl/config.mk
include ${IUEROOT}/contrib/oxl/oxp/config.mk
include $(IUEROOT)/core/vpl/config.mk

MINI_PROG_SOURCES += vobgetframe.cxx

PROGRAM = glmovie
SOURCES += glmovie.cxx
SOURCES += make_index.cxx

################################
include $(IUEROOT)/config/top.mk
################################
