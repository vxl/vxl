include ${fmk_ROOT}/fmk_params.mk

fmk_USES := vxl

fmk_LIBRARY := vgl
fmk_VERSION := 1.0

fmk_LIB_SOURCES += vgl_polygon.cxx
fmk_LIB_SOURCES += vgl_polygon_scan_iterator.cxx

fmk_LIB_SOURCES += vgl_distance.cxx
fmk_LIB_SOURCES += vgl_clip.cxx
fmk_LIB_SOURCES += vgl_lineseg_test.cxx
fmk_LIB_SOURCES += vgl_triangle_test.cxx
fmk_LIB_SOURCES += vgl_polygon_test.cxx

#MANPAGE_SOURCES += vgl_point_2d.txx
#MANPAGE_SOURCES += vgl_homg_point_2d.txx
#MANPAGE_SOURCES += vgl_point_3d.txx
#MANPAGE_SOURCES += vgl_homg_point_3d.txx
#MANPAGE_SOURCES += vgl_plane_3d.txx
#MANPAGE_SOURCES += vgl_homg_plane_3d.txx
#MANPAGE_SOURCES += vgl_line_2d.txx
#MANPAGE_SOURCES += vgl_homg_line_2d.txx
#MANPAGE_SOURCES += vgl_box_2d.txx
#MANPAGE_SOURCES += vgl_box_3d.txx
#MANPAGE_SOURCES += vgl_homg_line_3d_2_points.txx

#fmk_SUBDIRS += algo

ifndef NOTEST
#fmk_SUBDIRS += tests
endif

include ${fmk_ROOT}/fmk_rules.mk
