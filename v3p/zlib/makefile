#   DESCRIPTION:
#
#   This is the zlib directory of v3p.  Current version is 1.1.3.

USES := # nothing
USE_STDC++ := 0

LIBRARY := z
VERSION := 1.1.3

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

################################
include ${IUEROOT}/config/top.mk
################################

DEFINES += -DHAVE_UNISTD_H
