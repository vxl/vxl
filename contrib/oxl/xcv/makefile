##########################################
include $(IUEROOT)/vxl/vgui/easy-client.mk
##########################################

USES += vxl oxl

PROGRAM = xcv
ald_libs += mvl vsl vcgl
ifneq (,$(strip $(wildcard $(IUEROOT)/oxl/mvox)))
DEFINES += -DHAS_MVOX
ald_libs += mvox
endif

SOURCES += xcv_image_tableau.cxx

SOURCES += xcv_file.cxx
SOURCES += xcv_display.cxx
SOURCES += xcv_processing.cxx
SOURCES += xcv_geometry.cxx
SOURCES += xcv_segmentation.cxx

SOURCES += xcv_multiview.cxx
SOURCES += xcv_twoview_manager.cxx
SOURCES += xcv_threeview_manager.cxx
SOURCES += xcv_tjunction.cxx

SOURCES += xcv.cxx

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
