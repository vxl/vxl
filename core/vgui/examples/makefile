USES += core
include ../config.mk
include ${IUEROOT}/core/vidl/config.mk

MINI_PROG_SOURCES += basic01_display_image.cxx
MINI_PROG_SOURCES += basic02_display_line.cxx
MINI_PROG_SOURCES += basic03_display_text.cxx
MINI_PROG_SOURCES += basic04_display_2d.cxx
MINI_PROG_SOURCES += basic05_menubar.cxx
MINI_PROG_SOURCES += basic06_popup.cxx
MINI_PROG_SOURCES += basic07_dialog.cxx
MINI_PROG_SOURCES += basic08_printing.cxx
MINI_PROG_SOURCES += basic09_mouse_position.cxx
MINI_PROG_SOURCES += basic10_draw_line.cxx
MINI_PROG_SOURCES += basic11_display_3d.cxx
MINI_PROG_SOURCES += example_blender_tableau.cxx
MINI_PROG_SOURCES += example_display_pyramid.cxx
MINI_PROG_SOURCES += example_draw_polygon.cxx
MINI_PROG_SOURCES += example_enhance_tableau.cxx
MINI_PROG_SOURCES += example_get_highlighted.cxx
MINI_PROG_SOURCES += example_multiple_windows.cxx
MINI_PROG_SOURCES += example_poly_tableau.cxx
MINI_PROG_SOURCES += example_satellite_tableau.cxx
MINI_PROG_SOURCES += example_vgui_observer.cxx
MINI_PROG_SOURCES += gtk1_display_image.cxx
MINI_PROG_SOURCES += gtk2_movie_player.cxx
MINI_PROG_SOURCES += gtk3_vidl.cxx

SUBDIRS += mfc_example

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifdef HAS_GLUT
MINI_PROG_SOURCES += glut1_raw.cxx
MINI_PROG_SOURCES += glut2_raw.cxx
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
