USES += vxl oxl

## vil_stream_url uses tcp sockets
USE_SOCKETS := 1

# Harris corner detector
MINI_PROG_SOURCES += run_osl_harris.cxx

#
MINI_PROG_SOURCES += run_osl_canny.cxx
MINI_PROG_SOURCES += topology_edges.cxx
MINI_PROG_SOURCES += break_lines.cxx
MINI_PROG_SOURCES += fit_lines.cxx

ald_libs := osl vul #vbl,vil are deduced from osl.

################################
include ../client.mk
include ${IUEROOT}/config/top.mk
################################
