##########################################
# include ${IUELOCALROOT}/config/usr_params.mk
##########################################

USES := gel vxl oxl v3p
# the following 5 are needed in conjunction with libvil:
USE_JPEG := 1
USE_TIFF := 1
USE_PNG  := 1
USE_NETLIB := 1
USE_SOCKETS := 1

IULIBS := -lvsrl -lvsl -lvnl-algo -lvnl -lvil -lvbl

# include ../../vil/vil_config.mk

MINI_PROG_SOURCES += dense_matcher_full.cxx	
MINI_PROG_SOURCES += make_3d_output.cxx

################################
include ${IUEROOT}/config/top.mk
################################
