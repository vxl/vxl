##########################################
# include ${IUELOCALROOT}/config/usr_params.mk
##########################################


USES := gel
USES += vxl
USES += v3p

VXL := 1

IULIBS := -lvcl
IULIBS += -lvbl
IULIBS += -lvnl
IULIBS += -lvsrl
IULIBS += -lvil	

IULIBS += -lvgui -lvnl-algo -lvcl -lvsl -lnetlib -ljpeg

ifdef HAS_GLUT
IULIBS += -lglut
endif

# include ../../vil/vil_config.mk
include $(IUEROOT)/vxl/vgui/easy-client.mk

include $(IUELOCALROOT)/config/top-params.mk


MINI_PROG_SOURCES += dense_matcher_full.cxx	
MINI_PROG_SOURCES += make_3d_output.cxx

# include $(IUELOCALROOT)/config/top-rules.mk

##########################################
include ${IUEROOT}/config/top.mk
##########################################



##########################################
# include ${IUEROOT}/config/usr_rules.mk
##########################################

