USES += vxl oxl
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk

TESTS += oxltest.cpp
ifdef WIN32
SOURCES += StdAfx.cpp
endif

IULIBS += -lmvl -lvgl -lvnl-algo -lvnl -lvbl -lvul

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
