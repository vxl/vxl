USES = vxl

MINI_PROG_SOURCES += example1.cxx
MINI_PROG_SOURCES += vcsl_tutor.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
# gcc 3.0 needs too much virtual memory with optimisation
FLAGS_example1.cxx += -O1
endif

IULIBS += -lvcsl -lvbl -lvnl

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
