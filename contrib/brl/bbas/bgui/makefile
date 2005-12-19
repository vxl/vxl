USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ${VXLROOT}/core/vgui/config.mk

LIBRARY := bgui
VERSION := 1.0

SOURCES += bgui_picker_tableau.cxx
MANPAGE_SOURCES += bgui_picker_tableau_sptr.h
SOURCES += bgui_vtol2D_tableau.cxx
MANPAGE_SOURCES += bgui_vtol2D_tableau_sptr.h
SOURCES += bgui_vtol2D_rubberband_client.cxx
SOURCES += bgui_vtol_soview2D.cxx
SOURCES += bgui_image_tableau.cxx
MANPAGE_SOURCES += bgui_image_tableau_sptr.h
MANPAGE_SOURCES += bgui_roi_tableau_make_roi.h
SOURCES += bgui_vsol_camera_tableau.cxx
MANPAGE_SOURCES += bgui_vsol_camera_tableau_sptr.h
SOURCES += bgui_vsol_soview2D.cxx
SOURCES += bgui_vsol2D_tableau.cxx
MANPAGE_SOURCES += bgui_vsol2D_tableau_sptr.h
SOURCES += bgui_histogram_tableau.cxx
MANPAGE_SOURCES += bgui_histogram_tableau_sptr.h
SOURCES += bgui_selector_tableau.cxx
MANPAGE_SOURCES += bgui_selector_tableau_sptr.h
SOURCES += bgui_bmrf_soview2D.cxx
SOURCES += bgui_bargraph_clipon_tableau.cxx
MANPAGE_SOURCES += bgui_bargraph_clipon_tableau_sptr.h
SOURCES += bgui_range_adjuster_tableau.cxx
MANPAGE_SOURCES += bgui_range_adjuster_tableau_sptr.h
SOURCES += bgui_graph_tableau.cxx
MANPAGE_SOURCES += bgui_graph_tableau_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
 SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
