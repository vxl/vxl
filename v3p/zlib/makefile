#   DESCRIPTION:
#
#   This is the zlib directory of v3p.  Current version is 1.1.4.

USES := # nothing
USE_STDC++ := 0

LIBRARY := z
VERSION := 1.1.4

SOURCES += adler32.c
SOURCES += compress.c
SOURCES += crc32.c
SOURCES += gzio.c
SOURCES += uncompr.c
SOURCES += deflate.c
SOURCES += trees.c
SOURCES += zutil.c
SOURCES += inflate.c
SOURCES += infblock.c
SOURCES += inftrees.c
SOURCES += infcodes.c
SOURCES += infutil.c
SOURCES += inffast.c
MANPAGE_SOURCES += inffixed.h
MANPAGE_SOURCES += zconf.h
MANPAGE_SOURCES += zlib.h

# With -O2 or -O3, this file segfaults at line 514 on HP:
FLAGS_deflate.c += -O1

################################
include ${IUEROOT}/config/top.mk
################################

DEFINES += -DHAVE_UNISTD_H
