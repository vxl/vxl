##########################################
include $(IUEROOT)/oxl/vgui/easy-client.mk
##########################################

USES += vxl oxl v3p

PROGRAM = xcv
ald_libs += mvl osl vgl


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

SOURCES += xcv_multiview_create_multiview_menu.cxx
SOURCES += xcv_picker_tableau.cxx
SOURCES += xcv_axes_tableau.cxx
SOURCES += xcv_axes_limits.cxx

ifeq "$(IU_COMPILER)" "egcs"
FLAGS_xcv_geometry.cxx += -ftemplate-depth-100
endif
ifeq "$(TJ_GCC295)" "1"
FLAGS_xcv_geometry.cxx += -ftemplate-depth-100
endif
ifeq "$(TJ_GCC30)" "1"
FLAGS_xcv_geometry.cxx += -O0
FLAGS_xcv_multiview.cxx += -O0 # otherwise gcc 3.0 needs too much virtual memory
endif

MANPAGE_SOURCES += xcv_mview_manager.h

MANPAGE_SOURCES += xcv_axes_tableau_sptr.h
MANPAGE_SOURCES += xcv_image_tableau_sptr.h
MANPAGE_SOURCES += xcv_picker_tableau_sptr.h

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
