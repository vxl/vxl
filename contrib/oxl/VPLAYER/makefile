USES = vxl oxl v3p

ifeq ($(OS),win32)
PROGRAM = VPLAYER

SOURCES += easy2D_loader.cxx
SOURCES += easy2D_sequencer.cxx
SOURCES += vplayer.cxx
SOURCES += vplayer_file.cxx
SOURCES += vplayer_geometry.cxx
SOURCES += vplayer_video.cxx

IULIBS = -lnafxcw -lallvxl -lalloxl -lopengl32 -lglu32 # -lmvox
endif

################################
include $(IUEROOT)/config/top.mk
################################
