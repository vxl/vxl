##########################################
include $(IUEROOT)/oxl/vgui/easy-client.mk
##########################################

USES += vxl oxl

PROGRAM = xcv
ald_libs += mvl vsl vcgl

SOURCES += xcv_image_tableau.cxx

SOURCES += xcv_file.cxx
SOURCES += xcv_display.cxx
SOURCES += xcv_processing.cxx
SOURCES += xcv_geometry.cxx
SOURCES += xcv_segmentation.cxx

SOURCES += xcv_multiview.cxx
SOURCES += xcv_multiview_create_multiview_menu.cxx
SOURCES += xcv_twoview_manager.cxx
SOURCES += xcv_threeview_manager.cxx
SOURCES += xcv_tjunction.cxx

SOURCES += xcv.cxx

ifeq "$(IU_COMPILER)" "egcs"
FLAGS_xcv_geometry.cxx += -ftemplate-depth-100
endif

ifeq "$(IU_COMPILER)" "gcc-2.95"
FLAGS_xcv_geometry.cxx += -ftemplate-depth-100
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
