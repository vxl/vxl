TRY_GLUT := 1
TRY_OPENGL := 1
TRY_X11 := 1
IULIBS += -lvgui -lQv
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vgl/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/vbl/config.mk
include ${IUEROOT}/vxl/vul/config.mk

# from vgui/impl:

TRY_GTK := 1
TRY_MFC := 1
TRY_QT := 1
TRY_GLUT := 1
include ${IUEROOT}/config/top-params.mk
include ${IUEROOT}/config/try.mk

ifeq ($(HAS_GTKGLAREA),1)
  USE_GTKGLAREA := 1
endif

ifeq ($(HAS_MFC),1)
  USE_MFC := 1
endif

ifeq ($(HAS_QT),1)
  USE_QT := 1
endif

ifeq ($(HAS_GLUT),1)
  USE_GLUT := 1
endif
