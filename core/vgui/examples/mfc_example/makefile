USES = vxl oxl
include ${IUELOCALROOT}/oxl/vgui/config.mk

ifeq ($(OS),win32)
 PROGRAM := mfc_example

 SOURCES += MainFrm.cpp
 SOURCES += StdAfx.cpp
 SOURCES += example_mfc.cpp
 SOURCES += example_mfcDoc.cpp
 SOURCES += example_mfcView.cpp
 MANPAGE_SOURCES += example_mfc.rc

 IULIBS = -lnafxcw -lopengl32 -lglu32
endif

################################
include ${IUEROOT}/config/top.mk
################################

