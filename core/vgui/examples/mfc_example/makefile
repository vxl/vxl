USES += core
include ../../config.mk

include ${IUEROOT}/config/identify-os.mk
ifeq ($(OS),win32)
 IULIBS += -lnafxcw -lopengl32 -lglu32

 PROGRAM := mfc_example

 SOURCES += MainFrm.cpp
 SOURCES += StdAfx.cpp
 SOURCES += example_mfc.cpp
 SOURCES += example_mfcDoc.cpp
 SOURCES += example_mfcView.cpp
 MANPAGE_SOURCES += example_mfc.rc
endif

################################
include ${IUEROOT}/config/top.mk
################################
