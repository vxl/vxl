ifndef vgui_client_mk
vgui_client_mk := 1
#
# fsm@robots.ox.ac.uk
#

USE_OPENGL := 1
TRY_GLUT := 1
TRY_HERMES := 1

include $(IUELOCALROOT)/vxl/vbl/client.mk
include $(IUELOCALROOT)/vxl/vul/client.mk
include $(IUELOCALROOT)/vxl/vnl/client.mk
include $(IUELOCALROOT)/vxl/vil/client.mk
include $(IUELOCALROOT)/vxl/vgl/client.mk

ald_libdeps += vgui:vpl,vbl,vul,vnl-algo,vil,vgl
ald_libdeps += vgui-SDL:vgui
ald_libdeps += vgui-glut:vgui
ald_libdeps += vgui-Fl:vgui
ald_libdeps += vgui-X:vgui
ald_libdeps += vgui-Xm:vgui,vgui-X
ald_libdeps += vgui-glX:vgui,vgui-X
ald_libdeps += vgui-glut:vgui
ald_libdeps += vgui-gtk:vgui
ald_libdeps += vgui-qt:vgui

endif
