#
# fsm@robots.ox.ac.uk
#

fmk_libdeps += fmk_libC:fmk_libc,fmk_libm
fmk_libdeps += vcl:fmk_libC

fmk_libdeps += vbl:vcl
fmk_libdeps += vtl:vcl
fmk_libdeps += vgl:vcl

fmk_libdeps += vil:vcl
ifeq (1,$(tpp_HASPKG_JPEG))
fmk_libdeps += vil:jpeg
endif
ifeq (1,$(tpp_HASPKG_TIFF))
fmk_libdeps += vil:tiff
endif
ifeq (1,$(tpp_HASPKG_PNG))
fmk_libdeps += vil:png
endif

fmk_libdeps += vnl:vcl
fmk_libdeps += vnl-algo:vnl,netlib
fmk_libdeps += netlib:fmk_libc,fmk_libm

fmk_libdeps += vsl:vcl,vil,vbl,vnl-algo

fmk_libdeps += vgui:vcl,vbl,vnl-algo,vil,vgl
fmk_libdeps += vgui:glut
ifeq (1,$(tpp_HASPKG_HERMES))
fmk_libdeps += vgui:Hermes
endif
fmk_libdeps += vgui-SDL:vgui,SDL
fmk_libdeps += vgui-Fl:vgui,fltk
fmk_libdeps += vgui-X:vgui,X11
fmk_libdeps += vgui-Xm:vgui,vgui-X
fmk_libdeps += vgui-glX:vgui,vgui-X
fmk_libdeps += vgui-glut:vgui,glut
fmk_libdeps += vgui-gtk:vgui,gtk
fmk_libdeps += vgui-qt:vgui,qt
