USES := vxl gel oxl
USE_OPENGL:=1

IMMEDIATE_LIBS := -lvidl -lvgui -lvil -lvul

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)

LIBRARY := jvid
VERSION := 1.0

SOURCES += jvx_manager.cxx
# needs the avi codec which works under MS-Windows
endif

SUBDIRS += bin

################################
include ${IUEROOT}/config/top.mk
################################
