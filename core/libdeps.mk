# fsm@robots.ox.ac.uk

# the vgui lot
fsm_libdeps += vgui:vil,vbl,vnl-algo
fsm_libdeps += vgui-SDL:vgui
fsm_libdeps += vgui-glut:vgui
fsm_libdeps += vgui-Fl:vgui
fsm_libdeps += vgui-X:vgui
fsm_libdeps += vgui-Xm:vgui
fsm_libdeps += vgui-glX:vgui
fsm_libdeps += vgui-glut:vgui
fsm_libdeps += vgui-gtk:vgui

# the super six
fsm_libdeps += vbl:vcl
fsm_libdeps += vil:vcl
fsm_libdeps += vnl:vcl
fsm_libdeps += vnl-algo:vnl,netlib
#fsm_libdeps += vnl-algo:vnl,linpack,minpack
fsm_libdeps += vgl:vcl
fsm_libdeps += vgl-algo:vnl,vgl
fsm_libdeps += vsl:vcl,vbl,vil

# Extras
fsm_libdeps += oxp:vil,vnl,vbl,vcl


ifeq ($(HAS_NATIVE_JPEG),1)
fsm_libdeps += vil:jpeg
else
fsm_libdeps += vil:JPEG
endif

ifeq ($(HAS_NATIVE_TIFF),1)
fsm_libdeps += vil:tiff
else
fsm_libdeps += vil:TIFF
endif

ifeq ($(HAS_PNG),1)
fsm_libdeps += vil:png
endif

# netlib
ifndef CENTRAL_LIBDIR
LIBDIRS += $(IUELOCALROOT_libbase)/Numerics/$(tj_libdir)
endif
