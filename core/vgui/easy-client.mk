ifndef vgui_easy_client_mk
vgui_easy_client_mk := 1

##########################################
include $(IUELOCALROOT)/oxl/vgui/client.mk
include ${IUEROOT}/config/top-params.mk
##########################################

ifeq ($(strip $(HAS_GLUT)),1)
USE_GLUT := 1
endif

ifeq ($(strip $(HAS_FLTK)),1)
USE_FLTK := 1
endif

ifeq ($(strip $(HAS_GTKGLAREA)),1)
USE_GTKGLAREA := 1
endif

ifeq ($(strip $(HAS_SDL)),1)
USE_SDL := 1
endif

ifeq ($(strip $(HAS_QGL)),1)
USE_QGL := 1
endif

ifeq ($(strip $(HAS_MOTIFGL)),1)
USE_MOTIFGL := 1
endif

ifeq ($(strip $(HAS_X11)),1)
USE_X11 := 1
DEFINES += -DHAS_X11
endif

endif
