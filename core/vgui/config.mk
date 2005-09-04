TRY_GLUT := 1
TRY_OPENGL := 1
TRY_X11 := 1
IULIBS += -lvgui -lQv
include ${VXLROOT}/core/vnl/config.mk
include ${VXLROOT}/core/vil/config.mk
include ${VXLROOT}/core/vil1/config.mk
include ${VXLROOT}/core/vgl/config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/core/vbl/config.mk
include ${VXLROOT}/core/vul/config.mk
INCDIRS += ${VXLROOT}/core/vgui/$(RELOBJDIR)/include

# from vgui/impl:

TRY_GTK := 1
TRY_MFC := 1
TRY_QT := 1
TRY_GLUT := 1
include ${VXLROOT}/config/top-params.mk
include ${VXLROOT}/config/try.mk

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
