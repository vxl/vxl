USES += contrib/brl contrib/brl/bbas core

LIBRARY := bwm
VERSION := 1.0

SOURCES += bwm_corr.cxx
MANPAGE_SOURCES += bwm_corr_sptr.h
SOURCES += bwm_menu.cxx
SOURCES += bwm_observable_mesh.cxx
MANPAGE_SOURCES += bwm_observable_mesh_sptr.h
SOURCES += bwm_observable_point.cxx
MANPAGE_SOURCES += bwm_observable_point_sptr.h
SOURCES += bwm_observable_polyline.cxx
MANPAGE_SOURCES += bwm_observable_polyline_sptr.h
SOURCES += bwm_observable_textured_mesh.cxx
MANPAGE_SOURCES += bwm_observable_textured_mesh_sptr.h
SOURCES += bwm_observer.cxx
SOURCES += bwm_observer_cam.cxx
SOURCES += bwm_observer_img.cxx
SOURCES += bwm_observer_mgr.cxx
SOURCES += bwm_observer_proj_cam.cxx
SOURCES += bwm_observer_rat_cam.cxx
SOURCES += bwm_observer_vgui.cxx
SOURCES += bwm_observer_video.cxx
SOURCES += bwm_popup_menu.cxx
SOURCES += bwm_process_mgr.cxx
SOURCES += bwm_site_mgr.cxx
SOURCES += bwm_site.cxx
MANPAGE_SOURCES += bwm_site_sptr.h
MANPAGE_SOURCES += bwm_tableau_sptr.h
SOURCES += bwm_tableau_cam.cxx
SOURCES += bwm_tableau_factory.cxx
SOURCES += bwm_tableau_img.cxx
SOURCES += bwm_tableau_mgr.cxx
SOURCES += bwm_tableau_proj_cam.cxx
SOURCES += bwm_tableau_rat_cam.cxx
SOURCES += bwm_tableau_text.cxx
SOURCES += bwm_tableau_video.cxx
SOURCES += bwm_texture_map_generator.cxx
SOURCES += bwm_world.cxx
MANPAGE_SOURCES += bwm_command.h
MANPAGE_SOURCES += bwm_command_macros.h
MANPAGE_SOURCES += bwm_command_sptr.h
MANPAGE_SOURCES += bwm_def.h
MANPAGE_SOURCES += bwm_input.h
MANPAGE_SOURCES += bwm_load_commands.h
MANPAGE_SOURCES += bwm_macros.h
MANPAGE_SOURCES += bwm_mesh_observable.h
MANPAGE_SOURCES += bwm_observable.h
MANPAGE_SOURCES += bwm_observable_sptr.h
MANPAGE_SOURCES += bwm_observer_coin3d.h
MANPAGE_SOURCES += bwm_observer_lidar.h
MANPAGE_SOURCES += bwm_observer_proj2d.h
MANPAGE_SOURCES += bwm_observer_proj2d_simple.h
MANPAGE_SOURCES += bwm_process_commands.h
MANPAGE_SOURCES += bwm_tableau.h
MANPAGE_SOURCES += bwm_tableau_coin3d.h
MANPAGE_SOURCES += bwm_tableau_lidar.h
MANPAGE_SOURCES += bwm_tableau_proj2d.h
SOURCES += process/bwm_process.cxx
SOURCES += process/bwm_site_process.cxx

SUBDIRS += algo io reg video

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests exe
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
