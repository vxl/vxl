USES += vxl gel oxl
USE_OPENGL := 1

IULIBS += -ljvid -lvgui -lvidl -loxp -losl -lmvl -lvil -lvul

# Needs the avi codec which works under MS-Windows
# I think we should ask for the microsoft compiler instead of $(OS);
# if you know how to do it, feel free to participate.
ifeq ($(OS),win32)
MINI_PROG_SOURCES += jplayer.cxx
endif

################################
include ${IUEROOT}/config/top.mk
################################
