USES += contrib/oxl core
include ${VXLROOT}/contrib/oxl/oxp/config.mk
include ${VXLROOT}/core/vgui/config.mk
include ${VXLROOT}/core/vidl_vil1/config.mk
include ${VXLROOT}/core/vul/config.mk

#######################################
include $(VXLROOT)/config/top-params.mk
#######################################

ifeq ($(OS),win32)
 PROGRAM = VPLAYER

 SOURCES += easy2D_loader.cxx
 SOURCES += easy2D_sequencer.cxx
 SOURCES += vplayer.cxx
 SOURCES += vplayer_file.cxx
 SOURCES += vplayer_geometry.cxx
 SOURCES += vplayer_video.cxx

 IULIBS = -lnafxcw  -lopengl32 -lglu32 -lvfw32 # -lmvox
endif

######################################
include $(VXLROOT)/config/top-rules.mk
######################################
