TRY_GTK := 1
TRY_MFC := 1
TRY_QT := 1
TRY_GLUT := 1
include ${IUEROOT}/config/top-params.mk
include ${IUEROOT}/config/try.mk

ifeq ($(HAS_GTKGLAREA),1)
  USE_GTKGLAREA := 1
  include ${IUEROOT}/oxl/vgui/impl/gtk/config.mk
endif

ifeq ($(HAS_MFC),1)
  USE_MFC := 1
  include ${IUEROOT}/oxl/vgui/impl/mfc/config.mk
endif

ifeq ($(HAS_QT),1)
  USE_QT := 1
  include ${IUEROOT}/oxl/vgui/impl/qt/config.mk
endif

ifeq ($(HAS_GLUT),1)
  USE_GLUT := 1
  include ${IUEROOT}/oxl/vgui/impl/glut/config.mk
endif
