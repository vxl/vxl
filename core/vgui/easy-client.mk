ifndef vgui_easy_client_mk
vgui_easy_client_mk := 1

##########################################
include $(IUELOCALROOT)/oxl/vgui/client.mk
include ${IUEROOT}/config/top-params.mk
##########################################

ifeq ($(strip $(HAS_GLUT)),1)
USE_GLUT := 1
LD_UNDEFS += vgui_glut_tag
ald_libs += vgui-glut
endif

ifeq ($(strip $(HAS_FLTK)),1)
USE_FLTK := 1
LD_UNDEFS += vgui_fltk_tag
ald_libs += vgui-Fl
endif

ifeq ($(strip $(HAS_GTKGLAREA)),1)
USE_GTKGLAREA := 1
LD_UNDEFS += vgui_gtk_tag
ald_libs += vgui-gtk
endif

ifeq ($(strip $(HAS_SDL)),1)
USE_SDL := 1
LD_UNDEFS += vgui_SDL_tag
ald_libs += vgui-SDL
endif

ifeq ($(strip $(HAS_QGL)),1)
USE_QGL := 1
LD_UNDEFS += vgui_qt_tag
ald_libs += vgui-qt
endif

ifeq ($(strip $(HAS_MOTIFGL)),1)
USE_MOTIFGL := 1
LD_UNDEFS += vgui_Xm_tag
ald_libs += vgui-Xm
endif

ifeq ($(strip $(HAS_X11)),1)
USE_X11 := 1
DEFINES += -DHAS_X11
endif

ifneq (,$(strip $(wildcard $(IUEROOT)/oxl/vgui/impl/glX)))
## Just having X11 on my system, doesn't imply that
## I want to compile or link against vgui-glX.
ifeq ($(strip $(HAS_X11)),1)
LD_UNDEFS += vgui_glX_tag
ald_libs += vgui-glX vgui-X
endif
endif

endif
