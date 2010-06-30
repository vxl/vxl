#   DESCRIPTION:
#
#   This is the bzlib2 directory of v3p.  Current version is 1.0.5.

USES := # nothing
USE_STDC++ := 0

LIBRARY := bzip2
VERSION := 1.0.5

SOURCES += decompress.c
SOURCES += bzlib.c
SOURCES += blocksort.c
SOURCES += compress.c
SOURCES += crctable.c
SOURCES += huffman.c
SOURCES += randtable.c
MANPAGE_SOURCES += bzlib_private.h

################################
include ${VXLROOT}/config/top.mk
################################

DEFINES += -D_FILE_OFFSET_BITS -DBZ_DEBUG=0
