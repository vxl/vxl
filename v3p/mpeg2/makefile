USES = # nothing
USE_STDC++ := 0

LIBRARY := mpeg2
VERSION := 1.0

# This is where configure will put config.h and inttypes.h
INCDIRS += $(RELOBJDIR)/include
INCDIRS += ./include

SOURCES += libmpeg2/decode.c
SOURCES += libmpeg2/header.c
SOURCES += libmpeg2/stats.c
SOURCES += libmpeg2/cpu_state.c
SOURCES += libmpeg2/slice.c
SOURCES += libmpeg2/motion_comp.c
SOURCES += libmpeg2/motion_comp_mlib.c
SOURCES += libmpeg2/motion_comp_mmx.c
SOURCES += libmpeg2/motion_comp_altivec.c
SOURCES += libmpeg2/idct.c
SOURCES += libmpeg2/idct_mlib.c
SOURCES += libmpeg2/idct_mmx.c
SOURCES += libmpeg2/idct_altivec.c
SOURCES += libvo/video_out.c
# not needed in VXL:
# SOURCES += libvo/video_out_x11.c
# SOURCES += libvo/video_out_sdl.c
# SOURCES += libvo/video_out_mga.c
SOURCES += libvo/video_out_null.c
SOURCES += libvo/video_out_pgm.c
SOURCES += libvo/yuv2rgb.c
SOURCES += libvo/yuv2rgb_mlib.c
SOURCES += libvo/yuv2rgb_mmx.c

# configure stuff:
xall:: $(RELOBJDIR)/include/config.h
$(RELOBJDIR)/include/config.h: configure
	PwD=`pwd` ; cd $(RELOBJDIR) ; $$PwD/configure .

################################
include ${IUEROOT}/config/top.mk
################################

# Always optimize this lib
debug = $(optimize)

