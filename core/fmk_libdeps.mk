#
# fsm@robots.ox.ac.uk
#
fmk_libdeps += lib_vcl:pkg_vxl

fmk_libdeps += lib_vcl:lib_fmkC
fmk_libdeps += lib_vpl:lib_vcl
fmk_libdeps += lib_vbl:lib_vcl
fmk_libdeps += lib_vtl:lib_vcl
fmk_libdeps += lib_vgl:lib_vcl

fmk_libdeps += lib_vil:lib_vcl
ifeq (1,$(tpp_HASPKG_JPEG))
fmk_libdeps += lib_vil:lib_jpeg
endif
ifeq (1,$(tpp_HASPKG_TIFF))
fmk_libdeps += lib_vil:lib_tiff
endif
ifeq (1,$(tpp_HASPKG_PNG))
fmk_libdeps += lib_vil:lib_png
endif

fmk_libdeps += lib_vnl:lib_vcl
fmk_libdeps += lib_vnl-algo:lib_vnl,lib_netlib
fmk_libdeps += lib_netlib:lib_fmkc,lib_fmkm

fmk_libdeps += lib_vsl:lib_vil,lib_vbl,lib_vnl-algo

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
fmk_libdeps += lib_vgui-gtk:lib_vgui,lib_gtk
fmk_libdeps += lib_vgui-qt:lib_vgui,lib_qt
