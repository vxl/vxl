USES = vxl oxl v3p
include $(IUEROOT)/vxl/vil/config.mk
include $(IUEROOT)/vxl/vnl/algo/config.mk
USE_OPENGL := 1
USE_GLUT := 1
TRY_MPEG := 1

MINI_PROG_SOURCES += vobgetframe.cxx

PROGRAM = glmovie
SOURCES += glmovie.cxx
SOURCES += make_index.cxx

IULIBS += -lvidl -loxp -lvnl-algo -lvnl -lvbl -lvpl -lvul -lvil

################################
include $(IUEROOT)/config/top.mk
################################

