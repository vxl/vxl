#
# fsm@robots.ox.ac.uk
#

fmk_libdeps += lib_vsl:lib_vil,lib_vbl,lib_vnl-algo,pkg_oxl

fmk_libdeps += lib_mvox:lib_mvl
fmk_libdeps += lib_oxp:lib_vbl,lib_vil,lib_vnl-algo
fmk_libdeps += lib_mvl:lib_vnl-algo,lib_vgl,lib_vbl,lib_vil
fmk_libdeps += lib_vrml:lib_vgl,lib_vnl,lib_vbl

fmk_libdeps += lib_mvox,lib_oxp,lib_mvl,lib_vrml:pkg_oxl

fmk_libdeps += lib_vgui:pkg_oxl
fmk_libdeps += lib_vgui:lib_vpl,lib_vbl,lib_vnl-algo,lib_vil,lib_vgl
fmk_libdeps += lib_vgui:lib_glut
ifeq (1,$(tpp_HASPKG_HERMES))
fmk_libdeps += lib_vgui:lib_Hermes
endif
fmk_libdeps += lib_vgui-SDL:lib_vgui,lib_SDL
fmk_libdeps += lib_vgui-Fl:lib_vgui,lib_fltk
fmk_libdeps += lib_vgui-X:lib_vgui,lib_X11
fmk_libdeps += lib_vgui-Xm:lib_vgui,lib_vgui-X
fmk_libdeps += lib_vgui-glX:lib_vgui,lib_vgui-X
fmk_libdeps += lib_vgui-glut:lib_vgui,lib_glut
fmk_libdeps += lib_vgui-gtk:lib_vgui,lib_gtkgl
fmk_libdeps += lib_vgui-qt:lib_vgui,lib_qt
